FROM debian 

RUN apt update -y

RUN apt install -y golang

RUN mkdir -p /usr/bin && mkdir -p /usr/share/webui/www

ADD server.go .
RUN go mod init webui
RUN GOOS=linux  go build 
RUN mv webui /usr/bin
ADD www /usr/share/webui/www

ENTRYPOINT ["/usr/bin/webui"]
