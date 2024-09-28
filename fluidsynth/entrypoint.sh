#!/bin/sh
if [ "${FONT_TITLE}" = "" ];then
  FONT_TITLE=$(head /soundfont |strings|grep -A1 INAM|grep -v INAM)
fi
echo Font $FONT_TITLE
pw-jack -p 256 fluidsynth -i -s -a jack -m jack -o midi.autoconnect=0 -o synth.dynamic-sample-loading=1 -o midi.jack.id="${FONT_TITLE}" /soundfont &
fluid-ui &
sleep infinity
