#!/bin/bash

version=$(./ncompass-version.sh | tr ' ' '.')
cd 'build/dist' && \
sha1sum sentinel.img > manifest.sha1
zip exe.fb manifest.sha1 sentinel.img
sha1=$(sha1sum exe.fb | awk '{print $1}')
if [ $DOCKER_SYSTEM_TARGET = "yocto" ]; then
	platform="NCOMPASS-PT206"
	zipplatform="PT206"
elif [ $DOCKER_SYSTEM_TARGET = "nexus-amd64" ]; then
	platform="NEXUS-PT"
	zipplatform="NEXUS"
else
	platform="NCOMPASS-LITE"
	zipplatform="LITE"
fi

echo "$result"

cat << EOF > firmware.properties
platforms=$platform
images=exe.fb

exe.fb.name=exe.fb
exe.fb.version=$version
exe.fb.file=exe.fb
exe.fb.sha1=$sha1
exe.fb.readonly=false
EOF

zipname="nCompass_EXE_v${version}_${zipplatform}.fw"
echo "${zipname}"
zip ${zipname} firmware.properties exe.fb

echo "removing temporary files"
rm manifest.sha1 firmware.properties exe.fb