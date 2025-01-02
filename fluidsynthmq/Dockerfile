##########################################
# Base image
##########################################
FROM debian:sid as base
ARG DEBIAN_FRONTEND=noninteractive
##########################################
# Build fluid-ui
##########################################
#FROM scratch AS build_fluidui
#COPY --from=base / /
#RUN apt update -y
#RUN apt install -y build-essential
#ADD fluid-ui /fluid-ui
#WORKDIR /fluid-ui
#RUN ./build.sh
#RUN mv fluid-ui /usr/bin
##########################################
# Final image
##########################################
#FROM scratch AS final
#COPY --from=base / /
RUN apt update -y
RUN apt install -y pipewire-jack
#RUN apt install -y fluidsynth
#COPY --from=build_fluidui /usr/bin/fluid-ui /usr/bin/fluid-ui
##########################################
# Build fluidsynth
##########################################
RUN apt install -y build-essential cmake
RUN apt install -y libreadline-dev 
RUN apt install -y git
RUN git clone https://github.com/FluidSynth/fluidsynth.git
RUN apt install -y libglib2.0-dev
RUN apt install -y libjack-jackd2-dev
WORKDIR fluidsynth
RUN mkdir build
WORKDIR build
RUN cmake  -DLIB_SUFFIX="" -DCMAKE_INSTALL_PREFIX=/usr ..
RUN make
RUN make install  
WORKDIR /
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

