package jalv
// #cgo LDFLAGS:  -lzix-0 -ljack -llilv-0 -lserd-0 -lsratom-0 -lsuil-0
// #cgo CFLAGS: -I/usr/include/zix-0 -I/usr/include/lilv-0 -I/usr/include/serd-0 -I/usr/include/sratom-0 -I/usr/include/sord-0 -I/usr/include/suil-0 
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

// CFLAGS: -I/usr/include/zix-0 -I/usr/include/lilv-0 -I/usr/include/serd-0 -I/usr/include/sratom-0 -I/usr/include/sord-0 -I/usr/include/suil-0 -I/usr/include/gtk-3.0 -I/usr/include/glib-2.0 -I/usr/lib/aarch64-linux-gnu/glib-2.0/include/ -I/usr/include/pango-1.0 -I/usr/include/harfbuzz  -I/usr/include/cairo -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/atk-1.0  
