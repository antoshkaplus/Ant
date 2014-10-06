#!/bin/sh

# assignment should be 

# consider to run this script from build directory
# so, current directory is build directory
# to get include files directory we need to extract it from first argument

PATH_TO_INCLUDE=$(dirname "$0") 

TO_LIB_PATH="/usr/local/lib/libant.a"

cp "./libant.a" $TO_LIB_PATH

INCLUDE_PATH="/usr/local/include"

cp -R $PATH_TO_INCLUDE"/include/ant" $INCLUDE_PATH
