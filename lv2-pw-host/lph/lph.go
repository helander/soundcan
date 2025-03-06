package lph
// #cgo LDFLAGS: -lpipewire-0.3 -llilv-0 -ldl -lsord-0 -lserd-0 -lsratom-0
// #cgo CFLAGS: -g -I/usr/include/pipewire-0.3 -I/usr/include/spa-0.2 -D_REENTRANT  -I/usr/include/lilv-0 -I/usr/local/include -I/usr/include/serd-0 -I/usr/include/sord-0 -I/usr/include/sratom-0
//#include "lph.h"
import "C"
import "unsafe"

func Init() {
	C.lph_init()
}

func Plugin(pluginUri string) unsafe.Pointer {
	return C.lph_plugin(C.CString(pluginUri))
}

func Preset(plugin unsafe.Pointer, presetUri string) unsafe.Pointer {
	return C.lph_preset(plugin, C.CString(presetUri))
}

func Instance(plugin unsafe.Pointer, preset unsafe.Pointer, instanceName string, sampleRate int){
	go func() {
		C.lph_thread_loop(plugin, preset, C.CString(instanceName), C.int(sampleRate))
	}()
}

