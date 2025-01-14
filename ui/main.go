package main

import (
	"html/template"
	"log"
	"net/http"
	"path/filepath"
	"strconv"

        "github.com/helander/soundcan/ui/midi"
)

var templates *template.Template

func init() {
	var err error
	templates, err = template.New("dummy").Funcs(functions).ParseGlob(filepath.Join("templates", "*.gohtml"))
	if err != nil {
		log.Printf("Error creating templates %v", err)
	}

	if templates != nil {
		http.HandleFunc("/", serveIndexTemplate)
		http.HandleFunc("/indexbody", serveIndexBodyTemplate)
		http.HandleFunc("/fluidsynth/{port}/selectbank/{channel}", serveFluidsynthSelectBank)
		http.HandleFunc("/fluidsynth/{port}/selectprogram/{channel}", serveFluidsynthSelectProgram)
	}
        http.HandleFunc("/port/{port}/midicc/{channel}/{control}",serveMidiCC)
	images := http.FileServer(http.Dir("ui/img"))
	http.Handle("/img/", http.StripPrefix("/img/", images))
}

func main() {
	http.ListenAndServe(":3300", logRequest(http.DefaultServeMux))
}

func logRequest(handler http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		log.Printf("%s %s %s\n", r.RemoteAddr, r.Method, r.URL)
		handler.ServeHTTP(w, r)
	})
}

func serveIndexTemplate(w http.ResponseWriter, r *http.Request) {
	err := templates.ExecuteTemplate(w, "indexpage", "")
	if err != nil {
		log.Printf("Error executing index template %v", err)
	}
}

func serveIndexBodyTemplate(w http.ResponseWriter, r *http.Request) {
	err := templates.ExecuteTemplate(w, "indexbody", "")
	if err != nil {
		log.Printf("Error executing indexbody template %v", err)
	}
}

func serveFluidsynthSelectBank(w http.ResponseWriter, r *http.Request) {
	port := r.PathValue("port")
	channel, _ := strconv.Atoi(r.PathValue("channel"))
	err := r.ParseForm()
	if err != nil {
                log.Printf("FluidsynthSelectBank service: parse form error %v", err)
                return
	}
        mididata := make([]byte,3)
        bank, _ := strconv.Atoi(r.Form.Get("bank"))
	lsb := bank & 0x7f
	msb := (bank >> 7) & 0x7f
        mididata[0] = 0xb0 | 0x0f & byte(channel)
        mididata[1] = 0 // CC0
        mididata[2] = byte(msb)
        midi.Send(port,mididata)
        mididata[1] = 32 // CC32 
        mididata[2] = byte(lsb)
        midi.Send(port,mididata)
	midi.SelectBank(port,bank)
        mididata = make([]byte,2)
        mididata[0] = 0xc0 | 0x0f & byte(channel)
        mididata[1] = byte(0)
        midi.Send(port,mididata)
	midi.SelectProgram(port,0)
	err = templates.ExecuteTemplate(w, "fluidsynthpatch", port)
	if err != nil {
		log.Printf("Error executing fluidsynthpatch template %v", err)
	}
}

func serveFluidsynthSelectProgram(w http.ResponseWriter, r *http.Request) {
	port := r.PathValue("port")
	channel, _ := strconv.Atoi(r.PathValue("channel"))
	err := r.ParseForm()
	if err != nil {
                log.Printf("FluidsynthSelectProgram service: parse form error %v", err)
                return
	}
        mididata := make([]byte,2)
        program, _ := strconv.Atoi(r.Form.Get("program"))
        mididata[0] = 0xc0 | 0x0f & byte(channel)
        mididata[1] = byte(program)
        midi.Send(port,mididata)
	midi.SelectProgram(port,program)
	err = templates.ExecuteTemplate(w, "fluidsynthpatch", port)
	if err != nil {
		log.Printf("Error executing fluidsynthpatch template %v", err)
	}
}

func serveMidiCC(w http.ResponseWriter, r *http.Request) {
	port := r.PathValue("port")
	channel, _ := strconv.Atoi(r.PathValue("channel"))
	control, _ := strconv.Atoi(r.PathValue("control"))
	err := r.ParseForm()
	if err != nil {
                log.Printf("midiCC service: parse form error %v", err)
                return
	}
        mididata := make([]byte,3)
        ccvalue, _ := strconv.Atoi(r.Form.Get("ccvalue"))
        mididata[0] = 0xb0 | 0x0f & byte(channel)
        mididata[1] = byte(control)
        mididata[2] = byte(ccvalue)
        //log.Printf("MIDI %v to port %s",mididata,port)
        midi.Send(port,mididata)
}
