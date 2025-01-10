package midi

import (
	"fmt"

	"github.com/xthexder/go-jack"
)

var (
	portOut	*jack.Port
	inputch	chan jack.MidiData
)

func Send(midiport string,data []byte) {
			md := jack.MidiData{}
			md.Time = 0
			md.Buffer = data
			fmt.Printf("\nSend %v to inputch[%s]",md,midiport)
			inputch <- md
			fmt.Printf("\nSent to inputch")
}

func process(nframes uint32) int {
	buf := portOut.MidiClearBuffer(nframes)
	select {
    		case event := <- inputch:
        		fmt.Println("received event", event)
			portOut.MidiEventWrite(&event, buf)
    		default:
        		//fmt.Println("no message received")
    	}

	return 0
}

func init() {
	inputch = make(chan jack.MidiData,100)

	client, status := jack.ClientOpen("Soundcan UI Bridge", jack.NoStartServer)
	if status != 0 {
		fmt.Println(jack.StrError(status))
		return
	}
	//defer client.Close()

	portOut = client.PortRegister("midi_out", jack.DEFAULT_MIDI_TYPE, jack.PortIsOutput, 0)

	if code := client.SetProcessCallback(process); code != 0 {
		fmt.Println("Failed to set process callback: ", jack.StrError(code))
		return
	}

	if code := client.Activate(); code != 0 {
		fmt.Println("Failed to activate client: ", jack.StrError(code))
		return
	}

	fmt.Println(client.GetName())
}
