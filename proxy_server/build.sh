#!/bin/bash

rm -rf build
rm -rf bin
cmake -B build
cmake --build build -j