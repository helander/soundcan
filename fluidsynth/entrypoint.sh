#!/bin/sh
files=$(find /soundfonts/*)
#pw-jack -p 256 fluidsynth -i -s -a jack -m jack  -o midi.autoconnect=0 -o synth.dynamic-sample-loading=1 $files &
pw-jack -p 256 fluidsynth -i -s -a jack -m jack  -o midi.autoconnect=0 -o synth.dynamic-sample-loading=1 -o midi.jack.id="fs" -f /router.cfg $files &
sleep infinity
