#!/bin/sh
export LV2_PATH=$LV2_PATH:/SFZ/PRESETS
#pw-jack -p 512 jalv -i  -n "salamander-16bit(sfizz1)" -l /SFZ/salamander-16bit.state http://sfztools.github.io/sfizz &
pw-jack -p 512 jalv -i  -n "salamander-24bit(sfizz2)" -l /SFZ/salamander-24bit.state http://sfztools.github.io/sfizz &
#pw-jack -p 256 jalv.gtk3  http://sfztools.github.io/sfizz &
sleep infinity
