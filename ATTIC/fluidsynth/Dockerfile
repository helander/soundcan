##########################################
# Base image
##########################################
FROM debian:sid 
ARG DEBIAN_FRONTEND=noninteractive
##########################################
# Packages
##########################################
RUN apt update -y
RUN apt install -y pipewire-jack
RUN apt install -y fluidsynth
##########################################
# Defult environment provided by image
##########################################
ENV XDG_RUNTIME_DIR=/tmp
##########################################
# Startup
##########################################
ADD entrypoint.sh /
ADD router.cfg /
ENTRYPOINT ["/entrypoint.sh"]

