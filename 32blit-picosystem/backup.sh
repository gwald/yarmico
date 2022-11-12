#!/bin/bash
set -x #echo on
echo set -e


PROJHOME=/home/dev/eclipse-workspace/pico
PROJDEPLOY=$PROJHOME/00-deploy

mkdir -p $PROJDEPLOY/HTML5/
mkdir -p $PROJDEPLOY/picosystem
mkdir -p $PROJDEPLOY/32blit
mkdir -p $PROJDEPLOY/PC/linux
mkdir -p $PROJDEPLOY/PC/windows


echo for logging errors, run
echo ./backup.sh > out.txt 2>&1

cd $PROJHOME
rm $PROJDEPLOY/picosystem/*
rm $PROJDEPLOY/32blit/*
rm $PROJDEPLOY/PC/linux/*
rm $PROJDEPLOY/PC/windows/*
rm $PROJDEPLOY/HTML5/*


pwd




echo 32blit -----------------------------------------------------
echo "#define _RELEASE_ 1" > /home/dev/eclipse-workspace/pico/build.h
echo "#undef _DEBUG_ " >> /home/dev/eclipse-workspace/pico/build.h
echo "#undef HARDWARE_TARGET " >> /home/dev/eclipse-workspace/pico/build.h
echo "#define HARDWARE_TARGET HARDWARE_TARGET_32BLIT_HANDHELD " >> /home/dev/eclipse-workspace/pico/build.h




cd build.32blit
make clean
pwd
make -j1 

echo make ; ll pico.blit ; 32blit install pico.blit ; 32blit launch pico.blit
echo build for 32blit  cmake .. -DCMAKE_TOOLCHAIN_FILE=$HOME/32blit-sdk/32blit.toolchain -D32BLIT_DIR=$HOME/32blit-sdk -DCMAKE_BUILD_TYPE=Release 


echo build twice just in case first has a moment of weakness!
make -j1 

if [ -f "pico.blit" ]; then
    echo "pico.blit exists."
    cp pico.blit $PROJDEPLOY/32blit/yarmico.blit
    ll $PROJDEPLOY/32blit/yarmico.blit
else 
    echo "pico.blit does not exist."
    pwd
    exit 1
fi


make clean
echo 32blit -----------------------------------------------------
pwd
cd ..
pwd


echo pico -----------------------------------------------------
echo "#define _RELEASE_ 1" > /home/dev/eclipse-workspace/pico/build.h
echo "#undef _DEBUG_ " >> /home/dev/eclipse-workspace/pico/build.h
echo "#undef HARDWARE_TARGET " >> /home/dev/eclipse-workspace/pico/build.h
echo "#define HARDWARE_TARGET HARDWARE_TARGET_PICOSYSTEM " >> /home/dev/eclipse-workspace/pico/build.h



cd build.pico
make clean
pwd
make -j1 


echo build twice just in case first has a moment of weakness!
make -j1 

if [ -f "pico.uf2" ]; then
    echo "pico.uf2 exists."
    cp pico.uf2 $PROJDEPLOY/picosystem/yarmico.uf2
    ll $PROJDEPLOY/picosystem/yarmico.uf2
else 
    echo "pico.uf2 does not exist."
    pwd
    exit 1
fi


make clean
echo pico -----------------------------------------------------
pwd
cd ..
pwd





echo linux  -----------------------------------------------------
echo "#define _RELEASE_ 1" > /home/dev/eclipse-workspace/pico/build.h
echo "#undef _DEBUG_ " >> /home/dev/eclipse-workspace/pico/build.h

# default to SDL echo "#define HARDWARE_TARGET HARDWARE_TARGET_32BLIT_HANDHELD " >> /home/dev/eclipse-workspace/pico/build.h

echo move build dir, instead of rebuilding everytime!


#cd audio
#./setupAudioFiles.sh 1
#cd ..

cd build.release
make clean
pwd
make -j1 


echo build twice just in case first has a moment of weakness!
make -j1 

if [ -f "pico" ]; then
    echo "pico exists."
    cp pico $PROJDEPLOY/PC/linux/yarmico.x86-x64
    ll $PROJDEPLOY/PC/linux/yarmico.x86-x64
else 
    echo "pico does not exist."
    pwd
    exit 1
fi


make clean

pwd
echo linux  -----------------------------------------------------
cd ..

pwd

echo win -----------------------------------------------------

cd build.windows
pwd
make clean
make -j1

#echo will fail on metadata
#cp ../_metadata.cpp  .
#echo rebuild



echo build twice just in case first has a moment of weakness!
make -j1 

if [ -f "pico.exe" ]; then
    echo "pico exists."   
	cp -a pico.exe $PROJDEPLOY/PC/windows/yarmico-x86-x64.exe
	ll $PROJDEPLOY/PC/windows/yarmico-x86-x64.exe
else 
    echo "pico does not exist."
    pwd
    exit 1
fi


make clean

pwd
echo win -----------------------------------------------------
cd ..


pwd

echo html5 -----------------------------------------------------

cd build.html5
pwd
make clean 
make -j1   || true

echo build twice just in case first has a moment of weakness!
make -j1   || true

echo remove and redo with my link.txt file - custom web html
rm CMakeFiles/pico.dir/pico.c.*; rm pico.*
cp  $PROJHOME/link.txt   $PROJHOME/build.html5/CMakeFiles/pico.dir/
make -j1

if [ -f "pico.html" ]; then
    echo "pico HTML exists."    
    cp pico.* $PROJDEPLOY/HTML5
    ll $PROJDEPLOY/HTML5
else 
    echo "pico HTML does not exist."
    pwd
    exit 1
fi





make clean
pwd

echo html5 -----------------------------------------------------
cd ..





pwd

echo zipping up -----------------------------------------------------

cd $PROJHOME/..
now="pico-yarmico-linux-$(date +"%m_%d_%Y_%s").tar.gz"

echo "$now"

echo compressing...

# rm -rf /home/dev/eclipse-workspace/test/audio/include/*

# 7z a "$now" -xr~/eclipse-workspace/test/audio ~/eclipse-workspace/test
tar -cvpzf "$now" --exclude='test/audio'  --one-file-system pico
mv "$now" /media/00-keep/pico-backup


echo "" > $PROJHOME/build.h


