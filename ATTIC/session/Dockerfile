FROM debian:trixie AS base

RUN apt update -y


ARG DEBIAN_FRONTEND=noninteractive
##########################################
# Runtime dependencies 
##########################################
RUN apt install -y pipewire-jack
RUN apt install -y zenity
RUN apt install -y lilv-utils
RUN apt install -y nano
##########################################
# Build dependencies 
##########################################
RUN apt install -y libjack-jackd2-dev libsndfile-dev
RUN apt install -y git build-essential cmake 
RUN apt install -y python3
RUN apt install -y libcurl4-openssl-dev 
RUN apt install -y libelf-dev 
RUN apt install -y liblilv-dev
RUN apt install -y lv2-dev 
RUN apt install -y libcairo2-dev 
RUN apt install -y libfontconfig1-dev 
RUN apt install -y libpango1.0-dev 
RUN apt install -y libx11-xcb-dev 
RUN apt install -y libxcb-cursor-dev 
RUN apt install -y libxcb-keysyms1-dev 
RUN apt install -y libxcb-util-dev 
RUN apt install -y libxcb-xkb-dev 
RUN apt install -y libxkbcommon-dev 
RUN apt install -y libxkbcommon-x11-dev 
RUN apt install -y ninja-build
RUN apt install -y meson
RUN apt install -y libabsl-dev  libabsl20230802
##########################################
# Build sfizz 
##########################################
WORKDIR /build
RUN git clone https://github.com/sfztools/sfizz-ui.git --recursive
WORKDIR /build/sfizz-ui
RUN cmake -G Ninja -B build -S . -D CMAKE_INSTALL_PREFIX=/usr  -D SFIZZ_JACK=ON -D SFIZZ_RENDER=OFF -D SFIZZ_SHARED=OFF -D PLUGIN_LV2=ON -D PLUGIN_LV2_UI=ON -D PLUGIN_PUREDATA=OFF -D PLUGIN_VST3=OFF -D SFIZZ_USE_SYSTEM_ABSEIL=ON
RUN cmake --build build  
RUN cmake --build build  
RUN cmake --install build 


########################################
# Build dependencies
########################################
RUN apt install -y golang
RUN apt install -y build-essential git
RUN apt install -y meson
RUN apt install -y libjack-jackd2-dev
RUN apt install -y libserd-dev
RUN apt install -y libsord-dev
RUN apt install -y libsratom-dev
RUN apt install -y libsuil-dev
RUN apt install -y liblilv-dev
########################################
# Build dependencies
########################################
WORKDIR /src
RUN git clone https://github.com/drobilla/jalv.git
WORKDIR /build/galv
RUN cp -r /src/jalv/src jalv
RUN rm jalv/jalv_gtk.c
RUN rm jalv/jalv_qt.cpp jalv/jalv_qt.hpp
RUN rm jalv/portaudio.c 
RUN sed -e s/features\.h/xfeatures.h/ -i jalv/features.c
RUN sed -e s/features\.h/xfeatures.h/ -i jalv/jalv.c
RUN sed -e s/features\.h/xfeatures.h/ -i jalv/jalv.h
RUN mv jalv/features.h jalv/xfeatures.h
COPY src ./
RUN go mod init github.com/helander/soundcan/galv
RUN go build .
RUN cp galv /usr/bin




RUN apt install -y jalv setbfree lv2-examples mda-lv2

##########################################
# Startup
##########################################
ADD entrypoint.sh /
ENTRYPOINT ["/entrypoint.sh"]
##########################################
# Environment 
##########################################
ENV DISPLAY=desktop:0
ENV XDG_RUNTIME_DIR=/tmp
ENV LV2_PATH=/usr/lib/lv2:/SFZ/PRESETS
