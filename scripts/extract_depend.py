#!/bin/bash
#cat ./makefile | grep "^#00#" | sed -e "s/^....\(.*\)/\1/g" >depend.py
cd /home/rene/programmes/opengl/video/astro/
cat ./makefile | grep "^#00#" | sed -e "s/^....\(.*\)/\1/g"

