FROM golang:1.23 

RUN apt update
RUN apt install -y libjack-jackd2-dev pipewire-jack

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
