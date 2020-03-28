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

filename = " -o /home/rene/Documents/astronomie/capture/2020-03-27/triplet du lion/triplet"
iso = " -i 800"
stime = " -t 120"
frames = " -F 20"
timeout = " --timeout=3"
form = " --timeout=3"
focus = " --file_format=DNG";
cmd = "/home/rene/Documents/astronomie/logiciel/k200d/pktriggercord-0.84.04/pktriggercord-cli"

command = cmd + filename + iso + stime + timeout + frames + form

os.system( command );

