#!/bin/sh
pw-jack -p 256 jalv-http -i -H 3101 http://gareus.org/oss/lv2/b_synth &
pw-jack -p 1024  jalv-http -i -H 3100 http://helander.network/plugins/lv2/setbfreecontrol &
sleep infinity
