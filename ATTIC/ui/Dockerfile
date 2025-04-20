FROM debian:sid

RUN apt update
RUN apt install -y golang
#RUN apt install -y libjack-jackd2-dev pipewire-jack
RUN apt install -y libpipewire-0.3-dev
RUN apt install -y pipewire
RUN apt install -y ca-certificates
COPY pw_midimq.c .
RUN gcc -Wall pw_midimq.c -o /usr/bin/pw-midimq $(pkg-config --cflags --libs libpipewire-0.3)


# The prebuild folder contains a version of the module sources that uses most or all of the dependencies
# Having this here (and not changing its contents) provides a cached version of build artifacts that significantly
# speeds up the real build stage. In case dependencies change, make prebuild a copy of src, and next build will
# take the full build time, but subsequent builds will be significantly shorter
WORKDIR prebuild
RUN go mod init github.com/helander/soundcan/ui
COPY prebuild ./
RUN go mod tidy
RUN go build .


WORKDIR build
RUN go mod init github.com/helander/soundcan/ui
COPY src ./
RUN go mod tidy
RUN go build .
RUN mv ui /usr/bin

WORKDIR /root

COPY img ./img/
COPY templates ./templates/
COPY entrypoint.sh ./

ENTRYPOINT ["/root/entrypoint.sh"]

ENV XDG_RUNTIME_DIR=/tmp

