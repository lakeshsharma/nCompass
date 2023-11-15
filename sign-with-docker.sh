#!/usr/bin/env bash
set -Eeuo pipefail

: "${DOCKER_SYSTEM_TARGET:?not set -- expected value like lite-i386, pt316-amd64, yocto, etc: DOCKER_SYSTEM_TARGET=lite-i386 $0 $*}"

if [ ! -s build/dist/sentinel.smd ]; then
	echo >&2 "error: missing 'build/dist/sentinel.smd' file -- you need to build Sentinel first!"
	exit 1
fi

flashFileCreatorImage='aristocrat/ncompass:flash-file-creator'
docker build --tag "$flashFileCreatorImage" - <<'EODF'
FROM mono:6.10-slim

RUN set -eux; \
	apt-get update; \
	apt-get install -y --no-install-recommends \
		unzip \
		wget \
	; \
	rm -rf /var/lib/apt/lists/*

RUN set -eux; \
	cd /opt; \
	wget -O FlashFileCreator.zip http://nexus.systems.aristocrat.com/repository/raw-snapshot/ncompass/FlashFileCreator.zip; \
	echo '8da3f9e9050281f8cbbe43f0f680a0e11527fb7a167e4d7127a4bc508f79935b *FlashFileCreator.zip' | sha256sum --strict --check -; \
	unzip FlashFileCreator.zip; \
	rm FlashFileCreator.zip; \
	mono /opt/FlashFileCreator/FloorLogixFlashFileCreatorConsole.exe /?

ENTRYPOINT ["mono", "/opt/FlashFileCreator/FloorLogixFlashFileCreatorConsole.exe"]
EODF

sentinelVersion="$(bash ./ncompass-version.sh)"
sentinelVersion="${sentinelVersion// /.}"

ncompassFirmwareVersion="$(< ncompass-firmware-version)"
if [ "$DOCKER_SYSTEM_TARGET" != 'yocto' ]; then
	osVersion="$ncompassFirmwareVersion"
else
	sdkImage="us-west1-docker.pkg.dev/cxs-docker-repository/cxs-docker-dev/ncompass/firmware:$ncompassFirmwareVersion"
	DOCKER_IMAGE_TARGET="$DOCKER_SYSTEM_TARGET-amd64"
	sdkImage="$sdkImage-${DOCKER_IMAGE_TARGET##*-}-sdk-${DOCKER_IMAGE_TARGET%%-*}"
	if ! docker image inspect "$sdkImage" &> /dev/null; then
		docker pull "$sdkImage"
	fi
	firmwareBranchLabel='com.aristocrat.systems.ncompass-yocto.branch'
	firmwareYoctoRelease="$(docker image inspect "$sdkImage" --format '{{ index .Config.Labels "com.aristocrat.systems.ncompass-yocto.release" }}')"
	[ -n "$firmwareYoctoRelease" ]
	osVersion="$firmwareYoctoRelease"
fi

versionString="Sentinel${sentinelVersion}"
osOnlyVersionString="OS${osVersion}"
osVersionString="${osOnlyVersionString}_${versionString}"

case "$DOCKER_SYSTEM_TARGET" in
	lite-*)
		versionString+='-lite'
		;;
esac

args=(
	# extra / prefixes on following paths are for the sake of Windows (to stop MinGW from "helpfully" converting them to C:\... style paths)
	--mount "type=bind,src=$PWD/build/dist,dst=//workdir"
	--workdir //workdir
	--user "$(id -u):$(id -g)"
)

docker run --rm "${args[@]}" "$flashFileCreatorImage" -CO sentinel.smd sentinel "$osVersionString" S3
mv -v build/dist/sentinel.smd "build/dist/$versionString.smd"
mv -v build/dist/sentinel.S3DA "build/dist/$versionString.s3da"

for dir in \
	full-os \
	full-os/old-layers-naming \
; do
	fullDir="build/dist/$dir"
	if [ -s "$fullDir/sentinel.smd" ]; then
		docker run --rm "${args[@]}" "$flashFileCreatorImage" -CO "$dir/sentinel.smd" "$dir/sentinel" "$osVersionString" S3
		mv -v "$fullDir/sentinel.smd" "$fullDir/$osVersionString.smd"
		mv -v "$fullDir/sentinel.S3DA" "$fullDir/$osVersionString.s3da"
	fi
	if [ -s "$fullDir/OS-only-sentinel.smd" ]; then
		docker run --rm "${args[@]}" "$flashFileCreatorImage" -CO "$dir/OS-only-sentinel.smd" "$dir/OS-only-sentinel" "$osOnlyVersionString" S3
		mv -v "$fullDir/OS-only-sentinel.smd" "$fullDir/$osOnlyVersionString.smd"
		mv -v "$fullDir/OS-only-sentinel.S3DA" "$fullDir/$osOnlyVersionString.s3da"
	fi
done
