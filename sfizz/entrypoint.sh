#!/bin/sh
#pw-jack -p 256 jalv -i  -n salamander-16bit -l /SFZ/salamander-16bit.state http://sfztools.github.io/sfizz &
pw-jack -p 256 jalv -i  -n salamander-24bit -l /SFZ/salamander-24bit.state http://sfztools.github.io/sfizz &
#pw-jack -p 256 jalv.gtk3  http://sfztools.github.io/sfizz &
sleep infinity
