FROM sqimage-build-platform AS build


RUN apt install -y libgraphviz-dev
RUN apt install -y libgirepository1.0-dev
RUN apt install -y libboost-dev
RUN apt install -y libgtk2.0-dev
RUN apt install -y libgtkmm-2.4-dev

WORKDIR /
RUN git clone https://github.com/drobilla/ganv.git
WORKDIR /ganv
RUN meson setup build
WORKDIR /ganv/build
RUN meson configure -Dprefix="/usr" -Dprefer_static="true" -Ddefault_library="static"
RUN meson install

WORKDIR /
RUN git clone https://github.com/drobilla/raul.git
WORKDIR /raul
RUN meson setup build
WORKDIR /raul/build
RUN meson configure -Dprefix="/usr" -Dprefer_static="true" -Ddefault_library="static"
RUN meson install

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
RUN git clone https://github.com/drobilla/suil.git
WORKDIR /suil
RUN meson setup build
WORKDIR /suil/build
RUN meson configure -Dprefix="/usr" -Dprefer_static="true" -Ddefault_library="static"
RUN meson install

WORKDIR /
RUN git clone https://github.com/drobilla/lilv.git
WORKDIR /lilv
RUN meson setup build
WORKDIR /lilv/build
RUN meson configure -Dprefix="/usr" -Dprefer_static="true" -Ddefault_library="static"
RUN meson install



WORKDIR /
RUN git clone https://github.com/drobilla/ingen.git
WORKDIR /ingen
RUN meson setup build
WORKDIR /ingen/build
RUN meson configure -Dprefix="/usr" -Dprefer_static="true" -Ddefault_library="static" -Dgui="enabled" -Djack="enabled"
RUN meson compile
RUN meson install --destdir=/package 

FROM scratch

COPY --from=build /package/usr /usr
