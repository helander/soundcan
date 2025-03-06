FROM debian:trixie AS base
ARG ARCH

RUN apt update -y

RUN apt install -y build-essential git
RUN apt install -y meson
RUN touch x
RUN apt install -y libjack-jackd2-dev
RUN apt install -y libserd-dev
RUN apt install -y libsord-dev
RUN apt install -y libsratom-dev
RUN apt install -y libsuil-dev
RUN apt install -y liblilv-dev
RUN apt install -y libgtk-3-dev
RUN apt install -y qtbase5-dev
RUN apt install -y qt6-base-dev

#RUN apt install -y pkg-config
#RUN apt install -y cmake
#RUN apt install -y libgtk2.0-dev
#RUN apt install -y libgtkmm-2.4-dev
#RUN apt install -y libgraphviz-dev
#RUN apt install -y libgirepository1.0-dev

#WORKDIR /ganv
#RUN meson setup build
#WORKDIR /ganv/build
#RUN meson configure -Dprefix="/usr"
#RUN meson install


#RUN apt install -y libboost-dev

#WORKDIR /
#RUN git clone https://github.com/drobilla/raul.git

#WORKDIR /raul
#RUN meson setup build
#WORKDIR /raul/build
#RUN meson configure -Dprefix="/usr"
#RUN meson install



#WORKDIR /
#RUN git clone https://github.com/drobilla/jalv.git
#WORKDIR /jalv

#RUN meson setup build
#WORKDIR /jalv/build
#RUN meson configure -Dprefix="/usr" -Dgtk3="enabled" -Dqt5="enabled" -Dqt6="enabled" -Dsuil="enabled" -Djack="enabled"
#RUN meson compile
#RUN meson install  
RUN apt install -y golang
RUN apt install -y jalv
RUN apt install -y nano

#RUN cp /usr/lib/aarch64-linux-gnu/jack/jalv.so /usr/lib/aarch64-linux-gnu/
#RUN apt install -y lv2-examples
#RUN apt install -y pipewire-jack

WORKDIR /galvbuild
COPY golang ./
RUN go build .
RUN cp galv /usr/bin

##########################################
# Defult environment provided by image
##########################################
#ENV LV2_PATH=/usr/lib/lv2
#ENV DISPLAY=desktop:0
#ENV XDG_RUNTIME_DIR=/tmp
##########################################
# Startup
##########################################
#ADD entrypoint.sh /
#ENTRYPOINT ["/entrypoint.sh"]


