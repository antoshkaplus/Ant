#!/bin/sh

dir="./../coverage"
if [ ! -d $dir ]; then
    mkdir $dir 
fi
gcovr -r .. -e "(include/gtest/*)|(test/gtest/*)" \
        --html --html-details -o $dir"/res.html"