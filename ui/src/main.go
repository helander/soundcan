package main

import (
	"html/template"
	"log"
	"net/http"
	"path/filepath"
	"strconv"

        "github.com/helander/soundcan/ui/fluidsynth"
        "github.com/helander/soundcan/ui/posixmq"
)

var templates *template.Template

func init() {
        var err error
        templates, err = template.New("dummy").Funcs(fluidsynth.Functions).ParseGlob(filepath.Join("templates", "*.gohtml"))
        if err != nil {
                log.Printf("Error creating templates %v", err)
        }
	fluidsynth.Include(templates)

	if templates != nil {
		http.HandleFunc("/", serveIndexTemplate)
		http.HandleFunc("/indexbody", serveIndexBodyTemplate)
	}
        http.HandleFunc("/midimq/{port}/midicc/{channel}/{control}",serveMqMidiCC)
	images := http.FileServer(http.Dir("ui/img"))
	http.Handle("/img/", http.StripPrefix("/img/", images))
}


func main(){
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

func serveMqMidiCC(w http.ResponseWriter, r *http.Request) {
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
	mq, err := posixmq.Open("/"+port, posixmq.O_WRONLY | posixmq.O_CREAT, 0666, nil)
	if err != nil {
		log.Fatal(err)
	}
	defer posixmq.Close(mq)
	posixmq.Send(mq,mididata, 0)
}
