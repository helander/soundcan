package midi

import (
	"log"

	"github.com/xthexder/go-jack"
)

type PortRecord struct {
   Name string
   Chan chan jack.MidiData
   Output *jack.Port
}

var (
	ports map[string]PortRecord
)


func Send(port string,data []byte) {
			mididata := jack.MidiData{}
			mididata.Time = 0
			mididata.Buffer = data
			log.Printf("Send %v to gochan for port %s",mididata,port)
			ports[port].Chan <- mididata
			log.Printf("Sent to gochan")
}

func process(nframes uint32) int {
	for _, port := range ports {
		buf := port.Output.MidiClearBuffer(nframes)
		select {
    			case event := <- port.Chan:
        			log.Printf("received event", event)
				port.Output.MidiEventWrite(&event, buf)
    			default:
        			//log.Printf("no message received")
    		}
	}

	return 0
}

func init() {
	portNames := []string{"fs9800"}

	client, status := jack.ClientOpen("Soundcan UI Bridge", jack.NoStartServer)
	if status != 0 {
		log.Printf("%v",jack.StrError(status))
		return
	}
	//defer client.Close()

	ports = make(map[string]PortRecord)
	for _, name := range portNames {
		port := PortRecord{}
		port.Name = name
		port.Chan = make(chan jack.MidiData,100)
		port.Output = client.PortRegister(name, jack.DEFAULT_MIDI_TYPE, jack.PortIsOutput, 0)
		ports[name] = port
	}

	if code := client.SetProcessCallback(process); code != 0 {
		log.Printf("Failed to set process callback: %v", jack.StrError(code))
		return
	}

	if code := client.Activate(); code != 0 {
		log.Printf("Failed to activate client: %v", jack.StrError(code))
		return
	}

}
