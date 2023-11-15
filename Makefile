# nCompass Makefile
#
# Invokes makefiles in Sentinel / SentinelUnitTests as generated from slickedit.
# Update the slickedit project first and re-generate the makefiles.
#

# this can be overridden via "make CFG=Debug"
# ("./build-with-docker.sh CFG=Debug")
export CFG ?= Release

# this should be inherited from "./build-with-docker.sh" (as in, "DOCKER_SYSTEM_TARGET=lite-i386 ./build-with-docker.sh ...")
DOCKER_SYSTEM_TARGET ?= yocto

# set SHELL to bash to avoid inconsistencies in /bin/sh implementations
SHELL := bash

# this gets us a variable that's either "i386", "amd64", or "yocto"
DOCKER_SYSTEM_ARCH := $(lastword $(subst -,$() $(),$(DOCKER_SYSTEM_TARGET)))
# for yocto, this will be "S5Upgrade.smd", for everything else it will be "S5Upgrade-ARCH.smd"
S5UPGRADE_SMD := $(if $(filter-out yocto,$(DOCKER_SYSTEM_ARCH)),S5Upgrade-$(DOCKER_SYSTEM_ARCH).smd,S5Upgrade.smd)

.PHONY: default
default:
	$(MAKE) clean
	$(MAKE) sentinel
	$(MAKE) test

# make sure we can "./build-with-docker.sh clean sentinel" and have them build in the right order even with "-jN"
.NOTPARALLEL:

.PHONY: clean
clean:
	# remove artifacts we create
	rm -rf \
		'${ROOTFS}' \
		./build \
		Sentinel/stdafx.h.gch \
		SentinelUnitTests/$(CFG)/*.gcda \
		SentinelUnitTests/$(CFG)/*.gcno
	
	# invoke sub-Makefiles to remove each project's artifacts
	$(MAKE) -C Sentinel -f Sentinel.mak cleanall
	$(MAKE) -C SentinelUnitTests -f SentinelUnitTests.mak cleanall
	$(MAKE) -C makesmd -f makesmd.mak cleanall
	
	$(MAKE) -C S7_Test_Tools/NVRAMTester -f NVRAMTester.mak cleanall
	$(MAKE) -C S7_Test_Tools/Test9BitnCompass -f Test9BitnCompass.mak cleanall
	$(MAKE) -C S7_Test_Tools/TestBattery -f TestBattery.mak cleanall
	$(MAKE) -C S7_Test_Tools/TestMaster -f TestMaster.mak cleanall
	
	# clean up extra artifacts the unit tests leave hanging
	rm -f \
		SentinelUnitTests/1.txt \
		SentinelUnitTests/2.txt \
		SentinelUnitTests/3.txt \
		SentinelUnitTests/44545454 \
		SentinelUnitTests/LITESRAM.bin

ROOTFS := /tmp/sentinelfs
SENTINEL_HOME := $(ROOTFS)/opt/ncompass
DIST := build/dist

# Docker for Windows interacts with "mksquashfs" in an odd way that often results in an invalid squashfs file
# so, to counteract that, we "mksquashfs" to this other directory, then move the final result to $DIST/
TEMP_SQUASHFS_DIR := /tmp/ncompass-sentinel-squashfs

# a few defaults that may not be set elsewhere
OBJCOPY ?= objcopy

COVERITY_PATH ?= /opt/cov-platform
.PHONY: coverity cov-configure cov-build cov-analyze cov-commit-defects
coverity:
	@rm -rf build/coverity
	$(MAKE) cov-configure
	$(MAKE) cov-build
	$(MAKE) cov-analyze
cov-configure:
	'$(COVERITY_PATH)/bin/cov-configure' \
		--config build/coverity/config.xml \
		--template \
		--comptype g++ \
		--compiler $(CXX) \
		-- $(CXXFLAGS)
cov-build:
	$(MAKE) -C Sentinel -f Sentinel.mak cleanall
	'$(COVERITY_PATH)/bin/cov-build' \
		--config build/coverity/config.xml \
		--dir build/coverity/intdir \
		-- $(MAKE) -C Sentinel -f Sentinel.mak
cov-analyze:
	'$(COVERITY_PATH)/bin/cov-analyze' \
		--config build/coverity/config.xml \
		--dir build/coverity/intdir \
		--all \
		--enable-fnptr \
		--enable-virtual \
		--enable-constraint-fpp \
		--jobs auto
cov-commit-defects:
	@# http://innd-coverity-01.ali.local/get-data-url.cgi (source of "--host" and "--dataport" values)
	@# we can't just use "--host innd-coverity-01.ali.local --port 80" because it returns a bare "dela-coverity-t1" which then fails to lookup...
	'$(COVERITY_PATH)/bin/cov-commit-defects' \
		--dir build/coverity/intdir \
		--host dela-coverity-t1.ali.local --dataport 9090 \
		$${NCOMPASS_COVERITY_USER:+--user "$$NCOMPASS_COVERITY_USER"} \
		$${NCOMPASS_COVERITY_PASSWORD:+--password "$$NCOMPASS_COVERITY_PASSWORD"} \
		--stream '$(NCOMPASS_COVERITY_STREAM)'

.PHONY: sentinel
sentinel: prepare
	echo Building ${BUILD_NUMBER}
	cd Sentinel && sh ../pch.sh
	$(MAKE) -C Sentinel -f Sentinel.mak
	
	# sentinel binary
	mkdir -p ${SENTINEL_HOME}
	bash ./ncompass-version.sh > ${SENTINEL_HOME}/version.txt
	mkdir -p ${SENTINEL_HOME}/bin
	cp Sentinel/$(CFG)/Sentinel ${SENTINEL_HOME}/bin
	${OBJCOPY} --only-keep-debug Sentinel/$(CFG)/Sentinel ${DIST}/Sentinel.debug
	cp Binaries/rbcom ${SENTINEL_HOME}/bin
	chmod +x ${SENTINEL_HOME}/bin/rbcom
	
ifdef NCOMPASS_DEBUG_IMAGE
	# debug bits
	$(MAKE) s7-test-tools
	cp \
		S7_Test_Tools/NVRAMTester/$(CFG)/NVRAMTester \
		S7_Test_Tools/Test9BitnCompass/$(CFG)/Test9BitnCompass \
		S7_Test_Tools/TestBattery/$(CFG)/TestBattery \
		S7_Test_Tools/TestMaster/$(CFG)/TestMaster \
		${SENTINEL_HOME}/bin
endif
	
	# TODO-PORT - Move default content to ${SENTINEL_HOME}/os or ${SENTINEL_HOME}/etc?
	# TODO-PORT - Use ${SENTINEL_HOME}/media for the SD media card?
	# media content
	mkdir -p ${SENTINEL_HOME}/media/default
	cp -r --no-preserve=mode media ${SENTINEL_HOME}

	# Copy angular files
	cp -r --no-preserve=mode .angular/* ${SENTINEL_HOME}/media

	# debug.txt
	mkdir -p ${SENTINEL_HOME}/logs

	# init script
	mkdir -p ${ROOTFS}/etc/init.d
	cp --no-preserve=mode Scripts/sentinel.sh ${ROOTFS}/etc/init.d
	chmod +x ${ROOTFS}/etc/init.d/sentinel.sh
	mkdir -p ${ROOTFS}/etc/rc5.d
	cd ${ROOTFS}/etc/rc5.d && ln -sf ../init.d/sentinel.sh S50sentinel

	#certs
	mkdir -p ${SENTINEL_HOME}/etc
	mkdir -p ${SENTINEL_HOME}/etc/nginx
	mkdir -p ${SENTINEL_HOME}/etc/certs
	cp --no-preserve=mode Authentication/Aristocrat.OASIS.SpeedMediaKey.pem ${SENTINEL_HOME}/etc/certs
	cp --no-preserve=mode Authentication/Aristocrat_Technologies_Local_Host_2019.crt ${SENTINEL_HOME}/etc/certs
	cp --no-preserve=mode Authentication/Aristocrat_Technologies_nConnect_Root_CA_2013.crt ${SENTINEL_HOME}/etc/certs
	cp --no-preserve=mode Authentication/Aristocrat_Technologies_nConnect_Root_CA_2017.crt ${SENTINEL_HOME}/etc/certs
	cp --no-preserve=mode Authentication/Paltronics.crt ${SENTINEL_HOME}/etc/certs
	cp --no-preserve=mode Authentication/localhost.crt ${SENTINEL_HOME}/etc/nginx
	cp --no-preserve=mode Authentication/localhost.key ${SENTINEL_HOME}/etc/nginx
	cp --no-preserve=mode Authentication/localhost.conf ${SENTINEL_HOME}/etc/nginx

	#scripts
	cp --no-preserve=mode Scripts/killchrome.sh ${SENTINEL_HOME}/bin/
	chmod +x ${SENTINEL_HOME}/bin/killchrome.sh
	
	# config
	cp --no-preserve=mode Data/*.xml ${SENTINEL_HOME}/bin
	
	# hosts file
	cp --no-preserve=mode Data/hosts ${SENTINEL_HOME}/bin
	
	# Pre-generated web diagnostics file(s).
	mkdir -p ${SENTINEL_HOME}/etc/www
	cp --no-preserve=mode ExternalFolders/tempIndex.html ${SENTINEL_HOME}/etc/www
	
	cp --no-preserve=mode Data/debug.txt ${SENTINEL_HOME}/logs/debug.txt

ifeq ($(DOCKER_SYSTEM_TARGET), yocto)
	# shared libs
	mkdir -p ${ROOTFS}/usr/lib
	cp CardReaderLayer/$(CFG)/CardReaderLayer.so ${ROOTFS}/usr/lib
	
	# init script
	mkdir -p ${ROOTFS}/etc/init.d
	cp --no-preserve=mode Scripts/sentinel.sh ${ROOTFS}/etc/init.d
	chmod +x ${ROOTFS}/etc/init.d/sentinel.sh
	mkdir -p ${ROOTFS}/etc/rc5.d
	cd ${ROOTFS}/etc/rc5.d && ln -sf ../init.d/sentinel.sh S50sentinel
else # debian-based
	# shared libs
	cp CardReaderLayer/$(CFG)/CardReaderLayer.so ${SENTINEL_HOME}/bin
	
	# system symlinks
	ln -svfT etc/www ${SENTINEL_HOME}/www
	ln -svfT ../mediadisk/custom ${SENTINEL_HOME}/media/custom
	ln -svfT ../mediadisk/SpeedMediaContent ${SENTINEL_HOME}/media/speedmediacontent
	ln -svfT /run/media/mmcblk3p1 ${SENTINEL_HOME}/mediadisk
endif
	
	# Create image
	rm -f ${DIST}/sentinel.img
	rm -rf '$(TEMP_SQUASHFS_DIR)'
	mkdir '$(TEMP_SQUASHFS_DIR)'
ifeq ($(DOCKER_SYSTEM_TARGET), yocto)
	mksquashfs ${ROOTFS} '$(TEMP_SQUASHFS_DIR)/sentinel.img' -all-root
else
	mksquashfs ${SENTINEL_HOME} '$(TEMP_SQUASHFS_DIR)/sentinel.img' -all-root
endif

	cp -v '$(TEMP_SQUASHFS_DIR)/sentinel.img' '$(DIST)/'
	
	# cleanup our temporary directory
	rm -rf '$(TEMP_SQUASHFS_DIR)'

	chmod 777 build-exe-firmware.sh
	./build-exe-firmware.sh

.PHONY: makesmd
makesmd: ${DIST}/sentinel.img
	# Make download smd file
	$(MAKE) -C makesmd -f makesmd.mak
	cd '$(DIST)' && \
		'$(CURDIR)/makesmd/$(CFG)/makesmd' \
			sentinel.img \
			'$(S5UPGRADE_SMD)' \
			sentinel.smd

.PHONY: makeskin
makeskin: 
	chmod 777 build-angular-skin.sh
	./build-angular-skin.sh

.PHONY: source
source:
	mkdir -p '$(DIST)'
	tar --create \
		--file '$(DIST)/source.tar.xz' \
		--auto-compress \
		--exclude '$(CFG)' \
		--exclude 'Debug' \
		--exclude 'Release' \
		--exclude 'SMDs' \
		--exclude 'build' \
		--exclude 'stdafx.h.gch' \
		--exclude '.git' \
		--exclude '.angular' \
		--exclude '.angularsrc/mocksentinel ' \
		--exclude '.angularsrc/.husky' \
		--exclude '.angularsrc/SkinImport' \
		--exclude '.angularsrc/src/assets' \
		--owner 'user:1000' \
		--group 'user:1000' \
		--mtime '2021-01-01T00:00:00Z' \
		--transform 's,^./,,' \
		--sort name \
		--verbose \
		.

.PHONY: test
test: prepare
	$(MAKE) -C SentinelUnitTests -f SentinelUnitTests.mak ARCH=x86_64 CXX=g++ COMMON_CXXFLAGS='--coverage' COMMON_LDFLAGS='--coverage'
	cd SentinelUnitTests && ./$(CFG)/SentinelUnitTests --gtest_output=xml:../build/test_details.xml
.PHONY: valgrind-test
valgrind-test: prepare
	$(MAKE) -C SentinelUnitTests -f SentinelUnitTests.mak ARCH=x86_64 CXX=g++ COMMON_CXXFLAGS='--coverage' COMMON_LDFLAGS='--coverage'
	cd SentinelUnitTests \
		&& exec 3>../build/valgrind.log \
		&& valgrind \
			--log-fd=3 \
			--tool=memcheck \
			--leak-check=full \
			--show-leak-kinds=all \
			--track-origins=yes \
			--fullpath-after='$(CURDIR)/SentinelUnitTests/../' \
			--fullpath-after='$(CURDIR)/' \
			./$(CFG)/SentinelUnitTests \
			--gtest_output=xml:../build/test_details.xml
.PHONY: lcov
lcov:
	@mkdir -p build/coverage
	lcov --capture --directory SentinelUnitTests/$(CFG) --output-file build/coverage/coverage-raw.info
	lcov --remove build/coverage/coverage-raw.info --output-file build/coverage/coverage.info '/usr/*'
	genhtml build/coverage/coverage.info --output-directory build/coverage

.PHONY: s7-test-tools
s7-test-tools: prepare
	$(MAKE) -C S7_Test_Tools/NVRAMTester -f NVRAMTester.mak
	$(MAKE) -C S7_Test_Tools/Test9BitnCompass -f Test9BitnCompass.mak
	$(MAKE) -C S7_Test_Tools/TestBattery -f TestBattery.mak
	$(MAKE) -C S7_Test_Tools/TestMaster -f TestMaster.mak

.PHONY: prepare
prepare:
	mkdir -p ./build/tests
	mkdir -p ${DIST}

