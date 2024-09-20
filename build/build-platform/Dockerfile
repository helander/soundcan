FROM debian:sid
ARG DEBIAN_FRONTEND=noninteractive

RUN apt update -y

RUN apt install -y build-essential git meson cmake pkg-config
RUN apt install -y libglib2.0-dev
RUN apt install -y libjack-jackd2-dev
RUN apt install -y lv2-dev

