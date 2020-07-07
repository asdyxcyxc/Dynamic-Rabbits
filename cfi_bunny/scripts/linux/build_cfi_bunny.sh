#!/bin/bash

cd $CFI_BUNNY_DIR/src 
echo `pwd`

mkdir build
cd build

if [ "$1" == "x86_32" ] ; then
  export CXXFLAGS=-m32
  export CFLAGS=-m32
  is_x86_32="true"

elif [ "$1" == "x86_64" ] ; then
  is_x86_32="false"
else
  echo "No Architecture specified at parameter 1! Exiting with error";
  exit 1;
fi

if [ "$2" == "Debug" ] ; then
  is_debug="true"
  echo "Is Debug: TRUE"
elif [ "$2" == "Release" ]; then
  is_debug="false"
else
  echo "Unknown compilation build option at parameter 2! Exiting with error";
  exit 1;
fi


cmake -DX86_32=$is_x86_32 -DCMAKE_BUILD_TYPE=$2 -DDynamoRIO_DIR=$DYNAMORIO_DIR/install/cmake/ -DDYNAMORIO_SOURCE_DIR=$DYNAMORIO_DIR -DDrMemoryFramework_DIR=$DRMEMORY_DIR/install/drmf -DTAINT_BUNNY_DIR=$TAINT_BUNNY_DIR  -DUTILITY_BUNNY_DIR=$UTILITY_BUNNY_DIR -DSLICER_BUNNY_DIR=$SLICER_BUNNY_DIR -DINSTRUMENTATION_BUNNY_DIR=$INSTRUMENTATION_BUNNY_DIR  ..
make -j 4

cd $CFI_BUNNY_DIR/src/unit_tests
mkdir build
cd build

cmake -DCMAKE_BUILD_TYPE=$2 -DDynamoRIO_DIR=$DYNAMORIO_DIR/install/cmake/ -DDYNAMORIO_SOURCE_DIR=$DYNAMORIO_DIR -DDrMemoryFramework_DIR=$DRMEMORY_DIR/install/drmf -DTAINT_BUNNY_DIR=$TAINT_BUNNY_DIR  -DUTILITY_BUNNY_DIR=$UTILITY_BUNNY_DIR -DSLICER_BUNNY_DIR=$SLICER_BUNNY_DIR -DINSTRUMENTATION_BUNNY_DIR=$INSTRUMENTATION_BUNNY_DIR -DCFI_BUNNY_DIR=$CFI_BUNNY_DIR  ..
make -j 4

bash $CFI_BUNNY_DIR/tools/db_cfi/scripts/linux/build_db_cfi.sh $1 $2


