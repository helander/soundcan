##########################################
# Base image
##########################################
FROM debian:trixie AS base
ARG DEBIAN_FRONTEND=noninteractive
##########################################
RUN apt update -y
RUN apt install -y pipewire-jack
RUN apt install -y qsampler
##########################################
RUN apt install -y subversion
RUN apt install -y libxml-parser-perl
##########################################
#RUN apt install -y g++ debhelper pkg-config automake libtool fakeroot libsndfile1-dev doxygen uuid-dev lv2-dev
RUN apt install -y pkg-config automake libtool libsndfile1-dev doxygen uuid-dev lv2-dev
RUN apt install -y build-essential
RUN apt install -y bison libjack-jackd2-dev flex libsqlite3-dev dssi-dev
RUN apt install -y libgig-dev
##########################################
# Build linuxsampler components
##########################################

RUN svn co https://svn.linuxsampler.org/svn/linuxsampler/trunk linuxsampler

WORKDIR /linuxsampler
COPY linuxsampler-aarch64.patch .
RUN svn patch linuxsampler-aarch64.patch .
RUN make -f Makefile.svn
RUN HAVE_UNIX98=1 ./configure --prefix="/usr"
RUN make
RUN make install


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


