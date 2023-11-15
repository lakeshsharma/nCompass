#!/usr/bin/env bash
set -eu

# Precompile the stdafx header to speed up the build
$CXX -std=c++11 -lstdc++ -v -g -c -I ../Sentinel -I ../SDK-Include -D _SENTINEL stdafx.h
