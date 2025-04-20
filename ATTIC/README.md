# soundcan
Linux music production components using containers(docker) and pipewire

Highly portable components. 

Mandatory platform requirements:
* docker (and docker-compose) (should work with podman but not tested)
* pipewire and wireplumber servers

One of the strengths with pipewire, is that it works as a plugin replacement for the jack audio server. Never again do you have to configure and start a jack server. Pipewire hides all of that internally. Further most popular linux distros nowadays comes with pipewire as the standard audio server/system.

Once you have a system with pipewire and docker-compose you just have to clone this repository on that system and you are ready for meeting soundcan.

Soundcan comes with a set of tools, instruments and effects. All of these components are delivered as docker-compose services including the build instructions for all used docker images.

Adding your own components avaiable from some standard linux distro is simple, and it does not have to be the same distro you are using on you host platform.

Optional platform requirements:
* RT (realtime) kernel for the platform

With a realtime kernel you are able to run more instrument instances on a given hardware system. In case you have a very powerful computer you might not encounter any problems with a standard kernel. So it is up to your requirements regarding types and amount of instrument instances and your available computing power that determines the need for an RT kernel.

Real life example: a Raspberry Pi 4 with Rasperry Pi OS and Fluidsynth. Test done with standard kernel and a RT kernel. Procedure: increase number of Fluidsynth instances until "bad sounds" occur due to XRUNs. All Fluidsynth instances was triggered simultaneously by the same MIDI sequence (playing heavily on the keyboard).
* with the standard kernel (PREEMPT) it was possible to have 7 Fluidsynth instances without "bad sounds"
* with the RT kernel (PREEMPT_RT) it was possible to have 11 Fluidsynth instances without "bad sounds"

Some people argue that today's standard kernels are good enough for music production; and they are, for certain configurations. But given a specific hardware platform you will always get more performance out of it with an RT kernel. But if the performance you get matches your requirements you are among the lucky ones ( that do not have to find an RT kernel or buy stronger hardware).
