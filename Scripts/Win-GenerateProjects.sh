#!/bin/bash
cd ..
mkdir Launch
mkdir Project
cd Project
mkdir Assets
cd ..
mkdir Binaries
cd Binaries
cmake ..
cd Binaries/Release
cp 'Editor.exe' ../../../Launch/
cp 'Runtime.exe' ../../../Launch/