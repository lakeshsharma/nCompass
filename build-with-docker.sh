#!/usr/bin/env bash
set -Eeuo pipefail

# Run nCompass build using Docker.
#
# You can optionally pass in a make target, ie
#
#       ./build-with-docker.sh test
#
# Or build for a different system target like Lite on Linux, ie
#
#       DOCKER_SYSTEM_TARGET=debian-i386 ./build-with-docker.sh

: "${DOCKER_SYSTEM_TARGET:?not set -- expected value like lite-i386, pt316-amd64, yocto, nexus-amd64 etc: DOCKER_SYSTEM_TARGET=lite-i386 $0 $*}"

# https://jenkins.systems.aristocrat.com/job/ncompass-linux-firmware/job/master/

# Allow to override the ncompass-firmware-verison on a per-platform basis, nexus needs Debian 12 but all others are currently Debian 9
if [ -f "ncompass-firmware-version-${DOCKER_SYSTEM_TARGET}" ]; then
	: "${NCOMPASS_FIRMWARE_VERSION:=$(< ncompass-firmware-version-${DOCKER_SYSTEM_TARGET})}"
fi
: "${NCOMPASS_FIRMWARE_VERSION:=$(< ncompass-firmware-version)}"

windows=
case "$(uname -o)" in
	Msys) windows=1 ;;
esac

_abs() {
	if [ -n "$windows" ] && command -v cygpath > /dev/null; then
		cygpath --absolute --windows "$@"
	else
		readlink -f "$@"
	fi
}

dir="$(_abs "$PWD")"

args=(
	--name ncompass-build-with-docker
	--hostname "docker-ncompass-$DOCKER_SYSTEM_TARGET"
	--init

	# see also --tty behavior below
	--interactive --rm

	# extra / prefixes on following paths are for the sake of Windows (to stop MinGW from "helpfully" converting them to C:\... style paths)

	--mount "type=bind,src=$dir,dst=//home/falcondev/tfs/nCompassTest"
	--mount "type=bind,src=$ANGULAR_ARTIFACTS,dst=//home/falcondev/tfs/nCompassTest/.angular"
	--mount "type=bind,src=$ANGULAR_SRC,dst=//home/falcondev/tfs/nCompassTest/.angularsrc"
	--mount "type=bind,src=$ARCHIVE_DIRECTORY,dst=//home/falcondev/tfs/nCompassTest/.archive"
	--workdir //home/falcondev/tfs/nCompassTest

	--env DOCKER_SYSTEM_TARGET
	--env NCOMPASS_DEBUG_IMAGE

	--env BUILD_NUMBER="${BUILD_NUMBER:-99}"
	--env MAKEFLAGS="-j4 ${MAKEFLAGS:-}"
)

# on Windows, UID/GID mapping in Docker containers doesn't matter (Docker handles that with their filesystem sharing)
if [ -n "$windows" ]; then
	args+=( --user root )
else
	user="$(id -u):$(id -g)"
	groups="$(id -G | xargs -rn1 echo --group-add)"
	args+=(
		--mount type=bind,src=//etc/passwd,dst=//etc/passwd,readonly
		--mount type=bind,src=//etc/group,dst=//etc/group,readonly
		--user "$user"
		$groups
	)
fi

if [ -n "${COVERITY_PATH:-}" ] && [ -d "$COVERITY_PATH" ]; then
	coverity="$(_abs "$COVERITY_PATH")"
	args+=(
		--mount "type=bind,src=$coverity,dst=//opt/cov-platform,readonly"
		--env COVERITY_PATH='//opt/cov-platform'
		--env NCOMPASS_COVERITY_STREAM
		--env NCOMPASS_COVERITY_USER
		--env NCOMPASS_COVERITY_PASSWORD

		# Coverity tries to do things with $HOME, which won't work normally because of the "/etc/passwd" bind mount above, so we need to explicitly create a writeable HOME for Coverity to use
		--tmpfs //tmp/home
		--env HOME=//tmp/home
	)
fi

dockerRun='docker run'
if [ -t 0 ] && [ -t 1 ]; then
	args+=( --tty )

	if [ -n "$windows" ] && command -v winpty > /dev/null; then
		dockerRun="winpty $dockerRun"

		# Docker Toolbox creates a "docker" function to disable Git for Windows MSYS path conversions, but we handle those all ourselves instead (and work around MSYS auto-path conversion) since we need to for Docker for Windows (where MSYS is not the expected usage)
		unset docker &> /dev/null || :
	fi
fi

DOCKER_IMAGE="us-west1-docker.pkg.dev/cxs-docker-repository/cxs-docker-dev/ncompass/firmware:$NCOMPASS_FIRMWARE_VERSION"
if [ "$DOCKER_SYSTEM_TARGET" = 'yocto' ]; then
	DOCKER_IMAGE_TARGET="$DOCKER_SYSTEM_TARGET-amd64"
else
	DOCKER_IMAGE_TARGET="$DOCKER_SYSTEM_TARGET"
fi

# nexus is EFI bootloader so the docker image is named a little different to the others
if [ "$DOCKER_SYSTEM_TARGET" = 'nexus-amd64' ]; then
	DOCKER_IMAGE="$DOCKER_IMAGE-amd64-efi-sdk-nexus"
else
	DOCKER_IMAGE="$DOCKER_IMAGE-${DOCKER_IMAGE_TARGET##*-}-sdk-${DOCKER_IMAGE_TARGET%%-*}"
fi

# allow running "bash build-with-docker.sh bash" to get an interactive shell in the container environment
# (otherwise, we assume arguments should be passed directly to "make")
if [ "${1:-}" != 'bash' ]; then
	set -- make "$@"
fi
exec $dockerRun "${args[@]}" "$DOCKER_IMAGE" bash .docker-entrypoint.sh -- "$@"
