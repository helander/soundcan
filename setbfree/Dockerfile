##########################################
# Base image
##########################################
FROM debian:bookworm as base
ARG DEBIAN_FRONTEND=noninteractive
##########################################
RUN apt update -y
RUN apt install -y pipewire-jack
RUN apt install -y setbfree
RUN apt install -y jalv
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


