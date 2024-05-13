#!/bin/bash
cd /home/rene/programmes/opengl/video/astro
make -j$(nproc) && gdb ./astro -ex "r -p -e video0 -e video1"
