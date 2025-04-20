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
        "github.com/helander/soundcan/ui/parameter"
        "github.com/helander/gopkg/posixmq"
)

var templates *template.Template

func init() {
        var err error
        templates, err = template.New("dummy").Funcs(template.FuncMap{"map": CreateMap, "value": parameter.ParamValue, "parameterset": parameter.Selected, "fluidfonts": fluidsynth.Fonts}).ParseGlob(filepath.Join("templates", "*.gohtml"))
        if err != nil {
                log.Printf("Error creating templates %v", err)
        }
	fluidsynth.Include(templates)

	if templates != nil {
		http.HandleFunc("/", serveIndexTemplate)
	}
        http.HandleFunc("/midimq/{port}/midicc/{channel}/{control}",serveMqMidiCC)
        http.HandleFunc("/midimq/{port}/drawbarmidicc/{channel}/{control}",serveMqDrawbarMidiCC)
        http.HandleFunc("PUT /parameterset/active",serveParametersetActive)
        http.HandleFunc("PUT /parameterset/stored",serveParametersetStored)
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
///////	fluidsynth.FetchFonts()
	err := templates.ExecuteTemplate(w, "indexpage", nil)
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
	ccvalue := r.Form.Get("ccvalue")
	value, _ := strconv.ParseFloat(ccvalue, 64)
        mididata[0] = 0xb0 | 0x0f & byte(channel)
        mididata[1] = byte(control)
        mididata[2] = byte(int(math.Round(value)))
	log.Printf("mqmidicc: send %v to port %s",mididata,port)
	mq, err := posixmq.Open("/"+port, posixmq.O_WRONLY | posixmq.O_CREAT, 0666, nil)
	if err != nil {
		log.Fatal(err)
	}
	defer posixmq.Close(mq)
	posixmq.Send(mq,mididata, 0)
	parameter.Assign(port+"/midicc/"+r.PathValue("control"), ccvalue)
}

func serveMqDrawbarMidiCC(w http.ResponseWriter, r *http.Request) {
        w.WriteHeader(http.StatusNoContent)
	port := r.PathValue("port")
	channel, _ := strconv.Atoi(r.PathValue("channel"))
	control, _ := strconv.Atoi(r.PathValue("control"))
	err := r.ParseForm()
	if err != nil {
                log.Printf("drawbarmidiCC service: parse form error %v", err)
                return
	}
        mididata := make([]byte,3)
	ccvalue := r.Form.Get("ccvalue")
	value, _ := strconv.ParseFloat(ccvalue, 64)
	//min, _ := strconv.ParseFloat(r.Form.Get("min"), 64)
	//max, _ := strconv.ParseFloat(r.Form.Get("max"), 64)
	//step, _ := strconv.ParseFloat(r.Form.Get("step"), 64)
        mididata[0] = 0xb0 | 0x0f & byte(channel)
        mididata[1] = byte(control)
        mididata[2] = byte(int(math.Round(127.0*(1.0-value/8.0))))
	log.Printf("mqdrawbarmidicc: send %v to port %s",mididata,port)
	mq, err := posixmq.Open("/"+port, posixmq.O_WRONLY | posixmq.O_CREAT, 0666, nil)
	if err != nil {
		log.Fatal(err)
	}
	defer posixmq.Close(mq)
	posixmq.Send(mq,mididata, 0)
	parameter.Assign(port+"/drawbarmidicc/"+r.PathValue("channel")+"/"+r.PathValue("control"), ccvalue)
}




func serveParametersetActive(w http.ResponseWriter, r *http.Request) {
	err := r.ParseForm()
	if err != nil {
                log.Printf("parametersetActive service: parse form error %v", err)
        	w.WriteHeader(http.StatusNoContent)
                return
	}
	parameterset := r.Form.Get("parameterset")
	log.Printf("parametersetActive: form %s    %v",parameterset,r.Form)
	parameter.Activate(parameterset)
	err = templates.ExecuteTemplate(w, "indexbody", nil)
	if err != nil {
		log.Printf("Error executing index body template %v", err)
	}
}

func serveParametersetStored(w http.ResponseWriter, r *http.Request) {
        w.WriteHeader(http.StatusNoContent)
	err := r.ParseForm()
	if err != nil {
                log.Printf("parametersetStored service: parse form error %v", err)
                return
	}
	parameterset := r.Form.Get("parameterset")
	log.Printf("parametersetStored: form %s    %v",parameterset,r.Form)
	parameter.Store(parameterset)
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

