#!/bin/bash

version=$(./ncompass-version.sh | tr ' ' '.')
cd ".angularsrc/SkinImport" && \
zip SkinImport-${version}.zip skinImport.json asset/*
mv SkinImport-${version}.zip ../../.archive/
