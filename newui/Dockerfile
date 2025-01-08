FROM golang:1.23 

WORKDIR /opt

COPY go.mod go.sum ./
RUN go mod download && go mod verify

COPY . .

RUN go install ./...


ENTRYPOINT ["/go/bin/ui"]

