#!/usr/bin/env bash
set -Eeuo pipefail
set -x

: "${DOCKER_SYSTEM_TARGET:?not set -- expected value like lite-i386, pt316-amd64, yocto, nexus-amd64, etc: DOCKER_SYSTEM_TARGET=lite-i386 $0 $*}"

if [ ! -s build/dist/sentinel.img ]; then
	echo >&2 "error: missing 'build/dist/sentinel.img' file -- you need to build Sentinel first!"
	exit 1
fi

ncompassFirmwareVersion="$(< ncompass-firmware-version)"

sdkImage="us-west1-docker.pkg.dev/cxs-docker-repository/cxs-docker-dev/ncompass/firmware:$ncompassFirmwareVersion"
if [ "$DOCKER_SYSTEM_TARGET" = 'yocto' ]; then
	DOCKER_IMAGE_TARGET="$DOCKER_SYSTEM_TARGET-amd64"
else
	DOCKER_IMAGE_TARGET="$DOCKER_SYSTEM_TARGET"
fi

# nexus is EFI bootloader so the docker image is named a little different to the others
if [ "$DOCKER_SYSTEM_TARGET" = 'nexus-amd64' ]; then
	sdkImage="$sdkImage-${DOCKER_IMAGE_TARGET##*-}-efi-sdk-nexus"
else
	sdkImage="$sdkImage-${DOCKER_IMAGE_TARGET##*-}-sdk-${DOCKER_IMAGE_TARGET%%-*}"
fi

if ! docker image inspect "$sdkImage" &> /dev/null; then
	docker pull "$sdkImage"
fi

if [ "$DOCKER_SYSTEM_TARGET" = 'yocto' ]; then
	firmwareBranchLabel='com.aristocrat.systems.ncompass-yocto.branch'
	firmwareYoctoRelease="$(docker image inspect "$sdkImage" --format '{{ index .Config.Labels "com.aristocrat.systems.ncompass-yocto.release" }}')"
	[ -n "$firmwareYoctoRelease" ]
	firmwareBuildNumber="${firmwareYoctoRelease##*.}" # "1.1-tianon_firmware.2" -> "2"
	firmwareUrlBase='https://jenkins.systems.aristocrat.com/job/docker-firmware-kgroth'
	#firmwareImgFilename="sentinel7-squashfs-image-$firmwareYoctoRelease.img.gz"
	firmwareImgFilename="disk.img.gz"
	osVersion="$firmwareYoctoRelease"
else
	firmwareBranchLabel='com.aristocrat.systems.linux-image-builder.branch'
	firmwareBuildNumber="${ncompassFirmwareVersion##*-}" # "2.0-tianon_pt336-10" -> "10"
	firmwareUrlBase='https://jenkins.systems.aristocrat.com/job/ncompass-linux-firmware'
	firmwareImgFilename="$DOCKER_SYSTEM_TARGET-$ncompassFirmwareVersion.img.gz"
	if [ "$DOCKER_SYSTEM_TARGET" = 'nexus-amd64' ]; then
		firmwareImgFilename="$DOCKER_SYSTEM_TARGET-$ncompassFirmwareVersion-amd64-efi.img.gz"
	fi
	osVersion="$ncompassFirmwareVersion"
fi
sentinelVersion="$(bash ./ncompass-version.sh)"
sentinelVersion="${sentinelVersion// /.}"
versionString="OS${osVersion}_Sentinel${sentinelVersion}"

firmwareBranch="$(docker image inspect "$sdkImage" --format '{{ index .Config.Labels "'"$firmwareBranchLabel"'" }}')"
[ -n "$firmwareBranch" ]

linuxImageBuilderVersion="$(docker image inspect "$sdkImage" --format '{{ index .Config.Labels "com.aristocrat.systems.linux-image-builder.lib-version" }}')"
[ -n "$linuxImageBuilderVersion" ]

linuxImageBuilderBuilder="us-west1-docker.pkg.dev/cxs-docker-repository/cxs-docker-dev/linux-image-builder:$linuxImageBuilderVersion-builder"
if ! docker image inspect "$linuxImageBuilderBuilder" &> /dev/null; then
	docker pull "$linuxImageBuilderBuilder"
fi

fwslash='/'
firmwareUrl="$firmwareUrlBase/job/${firmwareBranch//$fwslash/%252F}/$firmwareBuildNumber/artifact/$firmwareImgFilename"

rm -rf build/fw
mkdir -p build/fw
wget --no-check-certificate -O build/fw/base.img.gz "$firmwareUrl"

args=(
	# extra / prefixes on following paths are for the sake of Windows (to stop MinGW from "helpfully" converting them to C:\... style paths)
	--mount "type=bind,src=$PWD,dst=//workdir"
	--workdir //workdir
	--user "$(id -u):$(id -g)"
	--env HOME=//tmp
	--env DOCKER_SYSTEM_TARGET
)
if [ -t 0 ] && [ -t 1 ]; then
	args+=( --interactive --tty )
fi

docker run --rm "${args[@]}" --env "versionString=$versionString" "$sdkImage" bash -Eeuo pipefail -xc '
	set -vEeuo pipefail
	# Total number of sectors (6291456) not a multiple of sectors per track (63)!
	# Add mtools_skip_check=1 to your .mtoolsrc file to skip this test

	echo mtools_skip_check=1 > ~/.mtoolsrc

	pv < build/fw/base.img.gz | gunzip | dd of=build/fw/base.img conv=sparse bs=1M

	offset="$(parted --script --machine --align=none build/fw/base.img -- unit B print | awk -F: "\$1 == \"1\" { sub(/B$/, \"\", \$2); print \$2; exit }")"

	copyTarget="::/"
	if [ "$DOCKER_SYSTEM_TARGET" != "yocto" ]; then
		copyTarget="::/firmware/"
	fi

	mkdir build/fw/firmware
	mcopy -i "build/fw/base.img@@$offset" -n -v -o -s "$copyTarget*" build/fw/firmware/

	cp build/dist/sentinel.img build/fw/firmware/

	copyFiles=( build/fw/firmware/sentinel.img )
	if [ -f build/fw/firmware/manifest.sha1 ]; then
		# manifest.sha1 present and includes OS only files.
		# create os.fb zip from this
		( cd build/fw/firmware && zip -r ../os.fb * -x ./sentinel.img)
		( cd build/fw/firmware && sha1sum ./sentinel.img | tee -a ./manifest.sha1 )
		# Sentinel sorts this file, so we need to also
		sort build/fw/firmware/manifest.sha1 > build/fw/firmware/manifest.sha1.new
		mv build/fw/firmware/manifest.sha1.new build/fw/firmware/manifest.sha1
		copyFiles+=( build/fw/firmware/manifest.sha1 )
	else
		# No sha1 file, create it but exclude sentinel.img 
		( cd build/fw/firmware && find . -type f ! -path ./manifest.sha1 ! -path ./sentinel.img -exec sha1sum "{}" + | sort > manifest.sha1 )
		# create os.fb zip from this
		( cd build/fw/firmware && zip -r ../os.fb * -x ./sentinel.img )
		# add sentinel.img to manifest.sha1 and re-sort
		( cd build/fw/firmware && sha1sum ./sentinel.img | tee -a ./manifest.sha1 )
		# Sentinel sorts this file, so we need to also
		sort build/fw/firmware/manifest.sha1 > build/fw/firmware/manifest.sha1.new
	fi

	mcopy -i "build/fw/base.img@@$offset" -n -v -o -s "${copyFiles[@]}" "$copyTarget"

	mcopy -i "build/fw/base.img@@$offset" -n -v -o <(echo -n) ::/CLEAR_MEMORY

	mkdir -p build/dist/full-os
	(
		if [ $DOCKER_SYSTEM_TARGET = "yocto" ]; then
			# Extract OS version ---- OS1.2-tvernaz_zeus.12_Sentinel15.1.0.66
			OSVersion=$(echo "$versionString" | grep -oE "OS([0-9a-zA-Z_\-]+\.){2}[0-9]+" | grep -oE "[0-9]+" | tr "\n" "." | rev | cut -c 2- | rev)
			platform="NCOMPASS-PT206"
			zipplatform="PT206"
		else
			# Extract OS version ---- OS2.0-tvernaz_zeus-28_Sentinel15.1.0.67
			OSVersion=$(echo "$versionString" | grep -oE "OS([0-9]+\.)([0-9]+\-)[a-zA-Z_]+?\-([0-9]+)" | grep -oE "[0-9]+" | tr "\n" "." | rev | cut -c 2- | rev)
			if [[ "$DOCKER_SYSTEM_TARGET" == *"nexus"* ]]; then
				platform="NEXUS-PT"
				zipplatform="NEXUS"
			else
				platform="NCOMPASS-LITE"
				zipplatform="LITE"
			fi
		fi

		# Extract Sentinel version
		ExeVersion=$(echo "$versionString" | grep -oE "Sentinel([0-9]+\.){3}[0-9]+" | cut -c 9-)

		echo "OS Version: $OSVersion"
		echo "Sentinel Version: $ExeVersion"
		
	# Onelink firmware package creation here. Directory already contains everything including manifest.sha1.
		cd build/fw
		
	# Need to create exe.fb zip file and its associated manifest.sha1 file
		(cd firmware && sha1sum sentinel.img > manifest.sha1 )
		(cd firmware && zip ../exe.fb ./sentinel.img ./manifest.sha1 )

	# os package creation
		sha1os=$(sha1sum os.fb | awk "{print \$1}")

		cat << OSEOF > firmware.properties
platforms=$platform
images=os.fb

os.fb.name=os.fb
os.fb.version=${OSVersion}
os.fb.file=os.fb
os.fb.sha1=$sha1os
os.fb.readonly=false
OSEOF
		zipname="nCompass_OS_v${OSVersion}_${zipplatform}.fw"
		echo "${zipname}"
		zip ${zipname} firmware.properties os.fb
		mv -v ${zipname} "../dist/full-os/${zipname}"
		rm firmware.properties

	# os_exe package creation
		sha1exe=$(sha1sum exe.fb | awk "{print \$1}")
		cat << EXEEOF > firmware.properties
platforms=$platform
images=os.fb,exe.fb

os.fb.name=os.fb
os.fb.version=${OSVersion}
os.fb.file=os.fb
os.fb.sha1=$sha1os
os.fb.readonly=false

exe.fb.name=exe.fb
exe.fb.version=${ExeVersion}
exe.fb.file=exe.fb
exe.fb.sha1=$sha1exe
exe.fb.readonly=false
EXEEOF

		zipname="nCompass_OS_v${OSVersion}_EXE_v${ExeVersion}_${zipplatform}.fw"
		echo "${zipname}"
		zip ${zipname} firmware.properties os.fb exe.fb
		mv -v ${zipname} "../dist/full-os/${zipname}"


		echo "removing temporary files"
		rm firmware.properties os.fb exe.fb
	)
	gzip -vf build/fw/base.img
	mv -v build/fw/base.img.gz "build/dist/full-os/$versionString.img.gz"

'

# now that we've got a full firmware image, let's create a "full" upgrade SMD file that includes the OS files we just extracted
case "$DOCKER_SYSTEM_TARGET" in
	yocto) s5upgrade='S5Upgrade.smd' ;;
	*)     s5upgrade="S5Upgrade-${DOCKER_SYSTEM_TARGET##*-}.smd" ;; # "S5Upgrade-i386", "S5Upgrade-amd64"
esac
docker run --rm "${args[@]}" --env "s5upgrade=$s5upgrade" "$sdkImage" bash -Eeuo pipefail -xc '
	make -C makesmd -f makesmd.mak CFG=Release clean all
	makesmd="$(readlink -f makesmd/Release/makesmd)"
	(
		cd build/fw/firmware
		rm -f manifest.sha1 # makesmd will unconditionally recreate this file...
		"$makesmd" -r "$s5upgrade" sentinel.smd
	)
	mv -v build/fw/firmware/*.smd build/dist/full-os/
	(
		cd build/fw/firmware
		rm -f manifest.sha1 # makesmd will unconditionally recreate this file...
		rm sentinel.img
		"$makesmd" -r "$s5upgrade" OS-only-sentinel.smd
	)
	mv -v build/fw/firmware/*.smd build/dist/full-os/
'

rm -rf build/fw
