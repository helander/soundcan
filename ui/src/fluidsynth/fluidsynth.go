package fluidsynth

import (
        "html/template"
        "log"
	"net"
	"net/http"
//	"path/filepath"
	"time"

)

var templates *template.Template

func Include(t *template.Template) {
	templates = t
}

func init() {
	http.HandleFunc("POST /fluidsynth/{host}/{port}/setting/{setting}", postSettingHandler)
	http.HandleFunc("POST /fluidsynth/{host}/{port}/midicc/{channel}/{control}", postMidiccHandler)
	http.HandleFunc("POST /fluidsynth/{host}/{port}/selectfont/{channel}", postSelectFontHandler)
	http.HandleFunc("POST /fluidsynth/{host}/{port}/setbankpreset/{channel}", postSetBankPresetHandler)
	http.HandleFunc("POST /fluidsynth/{host}/{port}/setvolume/{channel}", postSetVolumeHandler)
}

func postSettingHandler(w http.ResponseWriter, r *http.Request) {
	host := r.PathValue("host")
	port := r.PathValue("port")
	setting := r.PathValue("setting")
        err := r.ParseForm()
        if err != nil {
                log.Printf("postSettingHandler service: parse form error %v", err)
                return
        }
        settingvalue := r.Form.Get("settingvalue")
	response, err := FluidsynthCommand(host, port, "set "+setting+" "+settingvalue)
	log.Printf("Error %v   Response %v %s", err, response,response)
}

func postMidiccHandler(w http.ResponseWriter, r *http.Request) {
	host := r.PathValue("host")
	port := r.PathValue("port")
	channel := r.PathValue("channel")
	control := r.PathValue("control")
        err := r.ParseForm()
        if err != nil {
                log.Printf("postMidiccHandler service: parse form error %v", err)
                return
        }
        ccvalue := r.Form.Get("ccvalue")
	response, err := FluidsynthCommand(host, port, "cc "+channel+" "+control+" "+ccvalue)
	log.Printf("Error %v   Response %v %s", err, response, response)
}

func postSetVolumeHandler(w http.ResponseWriter, r *http.Request) {
	host := r.PathValue("host")
	port := r.PathValue("port")
	channel := r.PathValue("channel")
        err := r.ParseForm()
        if err != nil {
                log.Printf("postSetVolumeHandler service: parse form error %v", err)
                return
        }
        volume := r.Form.Get("volume")
	FluidsynthCommand(host, port, "cc "+channel+" "+"7"+" "+volume)
	engineKey := EngineKey{}
	engineKey.Host = host
	engineKey.Port = port
	engine := engines[engineKey]
        log.Printf("postSetVolumeHandler host %s port %s channel %s  engine %v", host, port, channel, engine)
	channelRecord := engine.Channels[channel]
        log.Printf("postSetVolumeHandler channelRecord %v", channelRecord)
	channelRecord.Volume = volume
	engine.Channels[channel] = channelRecord 
        log.Printf("postSetVolumeHandler updated channelRecord %v", channelRecord)
}


func postSelectFontHandler(w http.ResponseWriter, r *http.Request) {
	host := r.PathValue("host")
	port := r.PathValue("port")
	channel := r.PathValue("channel")
        err := r.ParseForm()
        if err != nil {
                log.Printf("postSelectFontHandler service: parse form error %v", err)
                return
        }
        font := r.Form.Get("font")
	engineKey := EngineKey{}
	engineKey.Host = host
	engineKey.Port = port
	engine := engines[engineKey]
	channelRecord := engine.Channels[channel]
	channelRecord.Sfont = font
	engine.Channels[channel] = channelRecord 
	FluidsynthCommand(host, port, "select "+channel+" "+font+" 0 0") 
	err = templates.ExecuteTemplate(w, "fluidsynthchannel", channelRecord)
	if err != nil {
		log.Printf("Error executing fluidsynthchannel template %v", err)
	}
}

func postSetBankPresetHandler(w http.ResponseWriter, r *http.Request) {
	host := r.PathValue("host")
	port := r.PathValue("port")
	channel := r.PathValue("channel")
        err := r.ParseForm()
        if err != nil {
                log.Printf("postSetBankPresetHandler service: parse form error %v", err)
                return
        }
        bankpreset := r.Form.Get("bankpreset")
	bank := bankpreset[0:3]
	preset := bankpreset[4:7]
	engineKey := EngineKey{}
	engineKey.Host = host
	engineKey.Port = port
	engine := engines[engineKey]
	channelRecord := engine.Channels[channel]
	channelRecord.Bank = bank
	channelRecord.Preset = preset
	engine.Channels[channel] = channelRecord 
	FluidsynthCommand(host, port, "select "+channel+" "+channelRecord.Sfont+" "+bank+" "+preset) 
	err = templates.ExecuteTemplate(w, "fluidsynthchannel", channelRecord)
	if err != nil {
		log.Printf("Error executing fluidsynthchannel template %v", err)
	}
}

func FluidsynthCommand(host string, port string, command string) ([]byte, error) {
	log.Printf("Fluidsynth send: host %s port %s command %s", host, port, command)
	tcpAddr, err := net.ResolveTCPAddr("tcp", host+":"+port)
	if err != nil {
		return nil, err
	}

	conn, err := net.DialTCP("tcp", nil, tcpAddr)
	if err != nil {
		return nil, err
	}
	conn.SetReadDeadline(time.Now().Add(10 * time.Millisecond))

	_, err = conn.Write([]byte(command + "\n"))
	if err != nil {
		return nil, err
	}

	response := make([]byte, 0)
	reply := make([]byte, 8196)

	goon := true
	for goon {

		l, err := conn.Read(reply)
		response = append(response, reply[:l]...)
		if err != nil {
			goon = false
		}

	}

	conn.Close()
	return response, nil
}

