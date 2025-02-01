#!/bin/sh
pw-midimq setbfree &
pw-midimq foobar &

./go/bin/ui

sleep infinity
