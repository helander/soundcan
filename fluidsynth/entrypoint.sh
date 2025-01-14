#!/bin/sh
if [ "${FONT_TITLE}" = "" ];then
  FONT_TITLE=$(head /soundfont |strings|grep -A1 INAM|grep -v INAM)
fi
echo Font $FONT_TITLE
engine=$SOUNDCAN_NAME

echo "inst 1"|fluidsynth -a file -q /soundfont|grep ...- |sed 's/-/,/'|sed 's/ /,/'|awk -v engine=$engine -F ',' '{printf "%s,%s,%s,%s\n",engine,$1,$2,$3}' >patches.csv
sqlite3 /session/settings.db "delete from patches where port = \"$engine\""
sqlite3 /session/settings.db -cmd '.mode csv' '.import patches.csv patches'

pw-jack -p 256 fluidsynth -i -s -a jack -m jack -o synth.midi-bank-select=mma -o midi.autoconnect=0 -o synth.dynamic-sample-loading=1 -o midi.jack.id="${FONT_TITLE}" /soundfont &
fluid-ui &
sleep infinity
