#!/bin/sh

# Precompile the stdafx header for SentinelUnitTests to speed up the build.
# This can be run from the Sentinel or SentinelUnitTests directory.
/usr/bin/g++ -pthread -std=c++11 -lstdc++ -v -g -c -I ../Sentinel -I ../SDK-Include -I ./Utility -D _SENTINEL -D _UNIT_TEST -o "../SentinelUnitTests/stdafx.h.gch" ../Sentinel/stdafx.h
