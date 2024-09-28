##########################################
# Base image
##########################################
FROM debian:sid as base
ARG DEBIAN_FRONTEND=noninteractive
##########################################
# Build jalv-http
##########################################
FROM scratch as build_jalv
COPY --from=base / /
RUN apt update -y
RUN apt install -y build-essential git meson cmake pkg-config
RUN apt install -y libglib2.0-dev
RUN apt install -y libjack-jackd2-dev
RUN apt install -y lv2-dev
RUN git clone https://github.com/helander/jalv-http.git
WORKDIR /jalv-http
RUN meson setup build
WORKDIR /jalv-http/build
RUN meson configure -Dprefix="/usr" -Dgtk3="disabled" -Dqt5="disabled" -Dqt6="disabled" -Dsuil="disabled" -Djack="enabled" -Ddefault_library="static" -Dprefer_static="true" \
 -Dzix:default_library="static" -Dserd:default_library="static" -Dsord:default_library="static"  -Dsratom:default_library="static" \
  -Dlilv:default_library="static" \
 -Dc_args="-Wno-implicit"
RUN meson compile
RUN meson install  
##########################################
# Build setbfreecontrol
##########################################
FROM scratch AS build_setbfreecontrol
COPY --from=base / /
RUN apt update -y
RUN apt install -y build-essential
RUN apt install -y lv2-dev
ADD src src/
ADD ttl ttl/
ADD Makefile .
RUN make
RUN make install
##########################################
# Final image
##########################################
FROM scratch AS final
COPY --from=base / /
COPY --from=build_jalv /usr/bin/jalv /usr/bin/jalv-http
COPY --from=build_setbfreecontrol /usr/lib/lv2 /usr/lib/lv2
RUN apt update -y
RUN apt install -y pipewire-jack
RUN apt install -y setbfree
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


