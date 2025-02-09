##########################################
# Base image
##########################################
FROM debian:trixie AS base
ARG DEBIAN_FRONTEND=noninteractive
##########################################
RUN apt update -y
##########################################
# Runtime dependencies 
##########################################
RUN apt install -y pipewire-jack
RUN apt install -y zenity
RUN apt install -y lilv-utils
RUN apt install -y jalv
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
#RUN apt install -y clang-format
##########################################
# Build sfizz 
##########################################
RUN git clone https://github.com/sfztools/sfizz-ui.git --recursive
WORKDIR /sfizz-ui
RUN cmake -G Ninja -B build -S . -D CMAKE_INSTALL_PREFIX=/usr  -D SFIZZ_JACK=OFF -D SFIZZ_RENDER=OFF -D SFIZZ_SHARED=OFF -D PLUGIN_LV2=ON -D PLUGIN_LV2_UI=ON -D PLUGIN_PUREDATA=OFF -D PLUGIN_VST3=OFF -D SFIZZ_USE_SYSTEM_ABSEIL=ON
#RUN cmake -G Ninja -B build -S . -D CMAKE_INSTALL_PREFIX=/usr -D SFIZZ_JACK=OFF -D SFIZZ_RENDER=OFF -D SFIZZ_SHARED=OFF -D PLUGIN_LV2=ON -D PLUGIN_LV2_UI=ON -D PLUGIN_PUREDATA=OFF -D PLUGIN_VST3=OFF 
RUN cmake --build build  
RUN cmake --install build 
##########################################
# Defult environment provided by image
##########################################
ENV LV2_PATH=/usr/lib/lv2
ENV DISPLAY=desktop:0
ENV XDG_RUNTIME_DIR=/tmp
##########################################
# Startup
##########################################
ADD entrypoint.sh /
ENTRYPOINT ["/entrypoint.sh"]
