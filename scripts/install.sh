#!/bin/sh

# assignment should be 

# consider to run this script from build directory
# so, current directory is build directory
# to get include files directory we need to extract it from first argument

TO_LIB_PATH="/usr/local/lib/libant.a"

cp "../lib/libant.a" $TO_LIB_PATH

INCLUDE_PATH="/usr/local/include"

cp -R "../include/ant" $INCLUDE_PATH
