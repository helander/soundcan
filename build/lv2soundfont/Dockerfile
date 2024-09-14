FROM sqimage-jalv-http

RUN apt install -y cmake
RUN apt install -y libglib2.0-dev
RUN apt install -y lv2-dev

WORKDIR /
RUN git clone https://github.com/FluidSynth/fluidsynth.git

WORKDIR /fluidsynth                                                                                                                                                                                 
RUN mkdir /fluidsynth/build                                                                                                                                                                                    
WORKDIR /fluidsynth/build                                                                                                                                                                                      
RUN cmake \
-Denable-aufile=0 \
-Denable-dbus=0 \
-Denable-ipv6=0 \
-Denable-jack=0 \
-Denable-ladspa=0 \
-Denable-libinstpatch=0 \
-Denable-libsndfile=0 \
-Denable-midishare=0 \
-Denable-opensles=0 \
-Denable-oboe=0 \
-Denable-network=0 \
-Denable-oss=0 \
-Denable-dsound=0 \
-Denable-wasapi=0 \
-Denable-waveout=0 \
-Denable-winmidi=0 \
-Denable-sdl2=0 \
-Denable-pulseaudio=0 \
-Denable-pipewire=0 \
-Denable-readline=0 \
-Denable-threads=1 \
-Denable-openmp=0 \
-Denable-alsa=0 \
-Denable-systemd=0 \
  -DLIB_SUFFIX="" -DCMAKE_INSTALL_PREFIX=/usr .. 
RUN make                                                                                                                                                                                           
RUN make install



WORKDIR /lv2soundfont
RUN mkdir src
ADD src src/
ADD Makefile .


RUN make prebuild

