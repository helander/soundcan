package main
import "time"
import "github.com/helander/golph/lph"
import "log"


func main() {
	lph.Init()
	//egs := lph.Plugin("http://lv2plug.in/plugins/eg-sampler")
//	b3 := lph.Plugin("http://gareus.org/oss/lv2/b_synth")
//	sfizz := lph.Plugin("http://sfztools.github.io/sfizz")
	liquidsfz := lph.Plugin("http://spectmorph.org/plugins/liquidsfz")
//	salamander16 := lph.Preset(sfizz,"file:///usr/lib/lv2/salamander16/preset.ttl")
//	uprightknight := lph.Preset(sfizz,"file:///usr/lib/lv2/uprightknight/preset.ttl")
	log.Printf("\nStarting")
//	lph.Instance(egs,"eg-sampler",48000)
//	lph.Instance(sfizz,nil,"nullpreset",48000)
	lph.Instance(liquidsfz,nil,"liquidsfz",48000)
//	lph.Instance(sfizz,salamander16,"salamander16",48000)
//	lph.Instance(sfizz,uprightknight,"uprightknight",48000)
//	lph.Instance(b3,nil,"Hammond B3",48000)
	for {time.Sleep(time.Minute)}
}
