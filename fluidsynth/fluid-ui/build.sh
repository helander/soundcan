#!/bin/sh

gcc -c fluid-ui.c -o fluid-ui.o
gcc -c fluid-controls.c -o fluid-controls.o
gcc -c fluid-tcp-client.c -o fluid-tcp-client.o
gcc -o fluid-ui fluid-ui.o fluid-controls.o fluid-tcp-client.o
