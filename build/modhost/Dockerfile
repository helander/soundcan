FROM sqimage-build-platform AS build

WORKDIR /
RUN git clone https://github.com/drobilla/serd.git
WORKDIR /serd
RUN meson setup build
WORKDIR /serd/build
RUN meson configure -Dprefix="/usr" -Dprefer_static="true" -Ddefault_library="static"
RUN meson install

WORKDIR /
RUN git clone https://github.com/drobilla/zix.git
WORKDIR /zix
RUN meson setup build
WORKDIR /zix/build
RUN meson configure -Dprefix="/usr" -Dprefer_static="true" -Ddefault_library="static"
RUN meson install

WORKDIR /
RUN git clone https://github.com/drobilla/sord.git
WORKDIR /sord
RUN meson setup build
WORKDIR /sord/build
RUN meson configure -Dprefix="/usr" -Dprefer_static="true" -Ddefault_library="static"
RUN meson install

WORKDIR /
RUN git clone https://github.com/drobilla/sratom.git
WORKDIR /sratom
RUN meson setup build
WORKDIR /sratom/build
RUN meson configure -Dprefix="/usr" -Dprefer_static="true" -Ddefault_library="static"
RUN meson install

WORKDIR /
RUN git clone https://github.com/drobilla/lilv.git
WORKDIR /lilv
RUN meson setup build
WORKDIR /lilv/build
RUN meson configure -Dprefix="/usr" -Dprefer_static="true" -Ddefault_library="static"
RUN meson install

##########################################
# Source code
##########################################
WORKDIR /
RUN git clone https://github.com/mod-audio/mod-host.git

RUN apt install -y libreadline-dev


##########################################
# Build & install
##########################################
WORKDIR /mod-host
RUN make PREFIX=/usr
RUN make install install PREFIX=/usr  

#FROM scratch

#COPY --from=build /usr/bin/mod-host /usr/bin/mod-host


