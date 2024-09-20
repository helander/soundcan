FROM sqimage-build-platform AS build

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
RUN meson configure -Dprefix="/usr" -Dgtk3="disabled" -Dqt5="disabled" -Dqt6="disabled" -Dsuil="disabled" -Djack="enabled" -Ddefault_library="static" -Dprefer_static="true" \
 -Dzix:default_library="static" -Dserd:default_library="static" -Dsord:default_library="static"  -Dsratom:default_library="static" \
  -Dlilv:default_library="static" \
 -Dc_args="-Wno-implicit"
#-Dlv2:default_library="static"
RUN meson compile
RUN meson install  

FROM scratch

COPY --from=build /usr/bin/jalv /usr/bin/jalv-http


