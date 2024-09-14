#!/bin/sh
Xvfb $DISPLAY -listen tcp &
sleep 5
x11vnc  -forever  &
sleep 5
xhost +
touch /ready
blackbox 
#startxfce4
