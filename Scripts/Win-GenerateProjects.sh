#!/bin/bash
cd ..
mkdir Launch
mkdir Binaries
cd Binaries
cmake ..
cd Binaries/Release
cp 'Editor.exe' ../../../Launch/
cp 'Runtime.exe' ../../../Launch/