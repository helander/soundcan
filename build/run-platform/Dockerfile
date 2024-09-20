FROM debian:sid
ARG DEBIAN_FRONTEND=noninteractive

RUN apt update -y

##########################################
# Libraries available in container
##########################################
RUN apt install -y libjack-jackd2-dev
RUN apt install -y libfluidsynth-dev
RUN apt install -y lv2-dev

##########################################
# Programs available in container
##########################################
RUN apt install -y lilv-utils
RUN apt install -y pipewire-jack
RUN apt install -y gcc

##########################################
# Defult environment provided by image
##########################################
ENV LV2_PATH=/usr/lib/lv2
ENV DISPLAY=desktop:0
ENV XDG_RUNTIME_DIR=/tmp


