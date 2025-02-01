FROM debian:sid

RUN apt update
RUN apt install -y golang
#RUN apt install -y libjack-jackd2-dev pipewire-jack
RUN apt install -y libpipewire-0.3-dev
RUN apt install -y pipewire

COPY pw_midimq.c .
RUN gcc -Wall pw_midimq.c -o /usr/bin/pw-midimq $(pkg-config --cflags --libs libpipewire-0.3)


COPY src ./build/
WORKDIR build
RUN go mod init github.com/helander/soundcan/ui
RUN go mod tidy
RUN go install ./...

WORKDIR /root

COPY img ./img/
COPY templates ./templates/
COPY entrypoint.sh ./

ENTRYPOINT ["/root/entrypoint.sh"]

ENV XDG_RUNTIME_DIR=/tmp
