#!/bin/bash
make -j$(nproc) && gdb ./astro -ex "r  -e video0 -e video1 -e video2"
