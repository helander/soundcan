FROM sqimage-lv2soundfont

WORKDIR /usr/lib/lv2/MonaLisa.lv2
ADD Monalisa-GM-v2_06_5.sf2 soundfont.sf2
RUN make -f /lv2soundfont/Makefile build

