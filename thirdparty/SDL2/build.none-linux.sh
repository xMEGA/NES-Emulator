#!/bin/bash

#set -x -e

THIS_SCRIPT_DIR=`pwd`

TARGET_PLATFORM=linux_x86_64
TARGET_CROSS_COMPILE=/usr/bin/
TARGET_SYSROOT_PATH=/
#_CFLAGS=$4


BUILD_DIR=${THIS_SCRIPT_DIR}/"build"/${TARGET_PLATFORM}

SDL="SDL2-2.0.3"

SOURCES_DIR=${SDL}

tar -zxvf ${SDL}'.tar.gz'

# Define host and target archs
# HOST="x86_64-none-linux-gnu"
# TARGET="arm-none-linux-gnueabi"

# Make sure $SOURCES_DIR directory exist
if test ! -d $SOURCES_DIR; then
  echo "$F error: source directory not found $SOURCES_DIR"
  exit 1
else
  echo "Source directory found"
fi

echo -e "Confuguration with parameters:\n*\n* \
	Platform: ${TARGET_PLATFORM}\n* \
	Compiler: ${TARGET_CROSS_COMPILE}\n*"


# Export copmpiler paths and  necessary 
export CC="${TARGET_CROSS_COMPILE}gcc"
export CXX="${TARGET_CROSS_COMPILE}gcc"
export LD="${TARGET_CROSS_COMPILE}gcc"
export AR="${TARGET_CROSS_COMPILE}ar"
export RANLIB="${TARGET_CROSS_COMPILE}ranlib"
#export CFLAGS=" -DNDEBUG -O2  \
#				-isysroot ${TARGET_SYSROOT_PATH} \
#				-I${TARGET_SYSROOT_PATH}/include ${_CFLAGS}"

cd $SOURCES_DIR/
./configure \
	--enable-static \
	--prefix="${BUILD_DIR}" 

make 
make install

cd ..

# Clean-up after building
rm -rf ${SOURCES_DIR}