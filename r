#!/bin/bash
cd /home/rene/programmes/opengl/video/astro


make -j$(nproc) && ./astro -p $* -e video0 -e video1
