FROM golang:1.23 

RUN apt update
RUN apt install -y libjack-jackd2-dev pipewire-jack

WORKDIR /opt

#COPY go.mod go.sum ./
RUN go mod init github.com/helander/soundcan/ui
RUN go mod tidy
RUN go get github.com/xthexder/go-jack 
RUN go get github.com/glebarez/go-sqlite
RUN go mod download && go mod verify

COPY . .

RUN go install ./...



ENV XDG_RUNTIME_DIR=/tmp

ADD entrypoint.sh /
ENTRYPOINT ["/entrypoint.sh"]

