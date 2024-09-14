# fluidweb
Web interface for fluidsynth

Requires TCP server port (default port number 9800) to be available on the fluidsynth instance.

Currently fluidweb should be installed and started  on the same host as where the controlled fluidsynth instance is running.
To deploy on some other host requires some (minor) modifications of the current code base.

* ./build.sh    # Results in packages in the repo sub directory (structured to be used as a repo for apt)

* in case you need other packaging than deb, please modify Dockerfile and build.sh

* to use with apt, copy fluidweb.list to your target's /etc/apt/sources.list.d folder and do apt update
*     wget https://raw.githubusercontent.com/helander/fluidweb/main/fluidweb.list



When done, please cleanup docker images on host
* docker rmi build-fluidweb-arm64
* docker rmi build-fluidweb-amd64


