package main

import (
	"errors"
	"html/template"
	"log"
	"math"
	"net/http"
	"path/filepath"
	"strconv"

        "github.com/helander/soundcan/ui/fluidsynth"
        "github.com/helander/gopkg/posixmq"
)

var templates *template.Template

func init() {
        var err error
        templates, err = template.New("dummy").Funcs(template.FuncMap{"map": CreateMap}).ParseGlob(filepath.Join("templates", "*.gohtml"))
        if err != nil {
                log.Printf("Error creating templates %v", err)
        }
	fluidsynth.Include(templates)

	if templates != nil {
		http.HandleFunc("/", serveIndexTemplate)
	}
        http.HandleFunc("/midimq/{port}/midicc/{channel}/{control}",serveMqMidiCC)
	images := http.FileServer(http.Dir("ui/img"))
	http.Handle("/img/", http.StripPrefix("/img/", images))
}


func main(){
	http.ListenAndServe(":3300",  logRequest(http.DefaultServeMux))
}


func logRequest(handler http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		log.Printf("%s %s %s\n", r.RemoteAddr, r.Method, r.URL)
		handler.ServeHTTP(w, r)
	})
}

func serveIndexTemplate(w http.ResponseWriter, r *http.Request) {
	fluidsynth.FetchFonts()
	err := templates.ExecuteTemplate(w, "indexpage", fluidsynth.Fonts)
	if err != nil {
		log.Printf("Error executing index template %v", err)
	}
}

func serveMqMidiCC(w http.ResponseWriter, r *http.Request) {
        w.WriteHeader(http.StatusNoContent)
	port := r.PathValue("port")
	channel, _ := strconv.Atoi(r.PathValue("channel"))
	control, _ := strconv.Atoi(r.PathValue("control"))
	err := r.ParseForm()
	if err != nil {
                log.Printf("midiCC service: parse form error %v", err)
                return
	}
        mididata := make([]byte,3)
	ccvalue, _ := strconv.ParseFloat(r.Form.Get("ccvalue"), 64)
        mididata[0] = 0xb0 | 0x0f & byte(channel)
        mididata[1] = byte(control)
        mididata[2] = byte(int(math.Round(ccvalue)))
	log.Printf("mqmidicc: send %v to port %s",mididata,port)
	mq, err := posixmq.Open("/"+port, posixmq.O_WRONLY | posixmq.O_CREAT, 0666, nil)
	if err != nil {
		log.Fatal(err)
	}
	defer posixmq.Close(mq)
	posixmq.Send(mq,mididata, 0)
}

func CreateMap(values ...interface{}) (map[string]interface{}, error) {
	if len(values)%2 != 0 {
		return nil, errors.New("invalid map call")
	}
	dict := make(map[string]interface{}, len(values)/2)
	for i := 0; i < len(values); i += 2 {
		key, ok := values[i].(string)
		if !ok {
			return nil, errors.New("map keys must be strings")
		}
		dict[key] = values[i+1]
	}
	return dict, nil
}
