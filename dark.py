#!/usr/bin/python3
#------------------------------------------------------------------------------------------------------------
#
#
#
#------------------------------------------------------------------------------------------------------------
from socket import *
import threading
import time


import struct
import math

import serial
import time

import threading
import sys, os

from tkinter import *
from time import sleep

import os

filename = " -o /home/rene/Documents/astronomie/logiciel/script/image/atmp/2000-01-01/k200d/m101-dark"
iso = " -i 800"
stime = " -t 180"
frames = " -F 12"
timeout = " --timeout=3"
focus = " -f";
cmd = "/home/rene/Documents/astronomie/logiciel/k200d/pktriggercord-0.84.04/pktriggercord-cli"

command = cmd + filename + iso + stime + timeout + frames

os.system( command );

