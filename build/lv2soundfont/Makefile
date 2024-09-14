#!/usr/bin/make -f


# ---------------------------------------------------------------------------------------------------------------------


# ---------------------------------------------------------------------------------------------------------------------


prebuild: \
	exporter \
        soundfont.o

build: \
	soundfont.so \
	manifest.ttl \
	soundfont.ttl 

SOUNDFONT=$(shell basename ${PWD} | awk -F '.' '{print $$1}')

# ---------------------------------------------------------------------------------------------------------------------
descriptor.c: /lv2soundfont/src/descriptor.c
	cat $< |sed s/FOOBAR/$(SOUNDFONT)/ > $@

descriptor.o: descriptor.c
	$(CC) -c $<  -o $@
	
soundfont.o: src/soundfont.c
	$(CC) -c $<  -o $@

soundfont.so: descriptor.o /lv2soundfont/soundfont.o
	$(CC) descriptor.o /lv2soundfont/soundfont.o   -DPIC -fPIC  -lfluidsynth -shared -o $@
	rm descriptor.c descriptor.o

soundfont.ttl: 
	sed "s/xLABELx/$(SOUNDFONT)/" /lv2soundfont/src/soundfont.ttl.p1 > soundfont.ttl
	/lv2soundfont/exporter 2>/dev/null >> soundfont.ttl
	sed "s/xLABELx/$(SOUNDFONT)/" /lv2soundfont/src/soundfont.ttl.p2 >> soundfont.ttl

manifest.ttl:
	sed "s/xLABELx/$(SOUNDFONT)/" /lv2soundfont/src/manifest.ttl.in > manifest.ttl

# ---------------------------------------------------------------------------------------------------------------------

exporter: src/exporter.c
	$(CC) $< -l fluidsynth  -o $@

# ---------------------------------------------------------------------------------------------------------------------
