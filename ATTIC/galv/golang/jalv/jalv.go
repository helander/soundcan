package jalv
// #cgo LDFLAGS:  -l:libzix-0.so -ljack -llilv-0 -lserd-0 -lsratom-0
// #cgo CFLAGS: -I/usr/include/zix-0 -I/usr/include/lilv-0 -I/usr/include/serd-0 -I/usr/include/sratom-0 -I/usr/include/sord-0
// extern void jalvmain();
import "C"

import "log"

func init() {
}

func JalvMain(){
	log.Printf("JalvMain enter")
	C.jalvmain()
	log.Printf("JalvMain exit")
}

// //#include "jalv.h"
// // #cgo LDFLAGS: -L /usr/lib/aarch64-linux-gnu/jack -l:jalv.so -l:libzix-0.so -ljack -llilv-0 -lserd-0 -lsratom-0
