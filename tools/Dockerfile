#Problem running qpwgraph in debian:trixie => do not use trixie (2024-08-25)
FROM debian
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update -y

##########################################
# Programs available in container
##########################################
RUN apt-get install -y qpwgraph
RUN apt-get install -y qtractor
RUN apt-get install -y vmpk
RUN apt-get install -y pipewire-jack
RUN apt-get install -y net-tools
RUN apt-get install -y xterm
RUN apt-get install -y lilv-utils

##########################################
# Defult environment provided by image
##########################################
ENV LV2_PATH=/usr/lib/lv2
ENV DISPLAY=desktop:0
ENV XDG_RUNTIME_DIR=/tmp


ADD entrypoint.sh /
RUN chmod a+x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]

