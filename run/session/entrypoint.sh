#!/bin/sh

install_soundfonts
pw-jack -p 256 jalv-http -i -H 7890 http://helander.network/lv2soundfont/essential-keys &
pw-jack -p 256 jalv-http -i -H 7891 http://helander.network/lv2soundfont/mona-lisa &
pw-jack -p 256 jalv-http -i  http://gareus.org/oss/lv2/b_synth &
pw-jack -p 256 jalv-http -i -H 7892 http://helander.network/plugins/lv2/setbfreecontrol &
sleep infinity
