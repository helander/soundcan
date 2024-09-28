##########################################
# Base image
##########################################
FROM debian:sid as base
ARG DEBIAN_FRONTEND=noninteractive
##########################################
# Build fluid-ui
##########################################
FROM scratch AS build_fluidui
COPY --from=base / /
RUN apt update -y
RUN apt install -y build-essential
ADD fluid-ui /fluid-ui
WORKDIR /fluid-ui
RUN ./build.sh
RUN mv fluid-ui /usr/bin
##########################################
# Final image
##########################################
FROM scratch AS final
COPY --from=base / /
RUN apt update -y
RUN apt install -y pipewire-jack
RUN apt install -y fluidsynth
COPY --from=build_fluidui /usr/bin/fluid-ui /usr/bin/fluid-ui
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

