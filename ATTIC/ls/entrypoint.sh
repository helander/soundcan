#!/bin/sh
###pw-jack -p 256 jalv -i  http://gareus.org/oss/lv2/b_synth
export LD_LIBRARY_PATH=/usr/local/lib/libgig
pw-jack linuxsampler &
qsampler
sleep infinity
