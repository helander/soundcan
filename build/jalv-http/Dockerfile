FROM debian

RUN apt update -y
##########################################
# Dependencies
##########################################
RUN apt install -y build-essential git
RUN apt install -y meson
RUN apt install -y libjack-jackd2-dev
RUN apt install -y libgtk-3-dev
RUN apt install -y qtbase5-dev
RUN apt install -y qt6-base-dev

##########################################
# Source code
##########################################
RUN git clone https://github.com/helander/jalv-http.git

##########################################
# Build & install
##########################################
WORKDIR /jalv-http
RUN meson setup build
WORKDIR /jalv-http/build
RUN meson configure -Dprefix="/usr" -Dgtk3="enabled" -Dqt5="enabled" -Dqt6="enabled" -Dsuil="enabled" -Djack="enabled"
RUN meson compile
RUN meson install  

##########################################
# Programs available in container
##########################################
RUN apt install -y lilv-utils
RUN apt install -y pipewire-jack

##########################################
# Defult environment provided by image
##########################################
ENV LV2_PATH=/usr/lib/lv2
ENV DISPLAY=desktop:0
ENV XDG_RUNTIME_DIR=/tmp


