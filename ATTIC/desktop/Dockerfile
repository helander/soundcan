FROM debian:sid
RUN apt update -y
RUN apt install -y xvfb
RUN apt install -y xterm
RUN apt install -y x11vnc
RUN apt install -y supervisor
RUN apt install -y blackbox 

ENV DISPLAY=:0.0
COPY conf.d /etc/supervisor/conf.d
COPY supervisord.conf /etc/supervisor/supervisord.conf
ENTRYPOINT supervisord
