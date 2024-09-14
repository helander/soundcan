FROM debian
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update -y
RUN apt-get install -y xvfb x11vnc blackbox menu
RUN apt-get install -y net-tools
RUN apt-get install -y x11-xserver-utils
# Alternative DE
#RUN apt-get install -y --no-install-recommends xfce4 dbus-x11

ADD entrypoint.sh /
RUN chmod a+x /entrypoint.sh

ENV DISPLAY :0

ENTRYPOINT ["/entrypoint.sh"]
