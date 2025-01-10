package main

import (
	"fmt"
	"html/template"
	"log"
	"net/http"
	"path/filepath"
	"strconv"


        "github.com/helander/soundcan/ui/db"
        "github.com/helander/soundcan/ui/midi"
)


type Engine struct {
  Kind string
  Controls map[string]Control
//  valuemaps ....
}

type Control struct {
  Engine string
  Kind string
  Name string
  Control db.ControlValue
}

var templates *template.Template

func getFullContext() map[string]Engine {
	context := make(map[string]Engine)

        engines, _ := db.GetEngines()
        //fmt.Printf("\nGetEngines  %v   %v", err,engines)

        for engine, kind := range engines {
          e := Engine{} 
          //fmt.Printf("\nengine %s    kind %s",engine,kind)
          e.Kind = kind
          controls, _ := db.GetControls(engine)
          e.Controls = make(map[string]Control)
          for control, value := range controls {
            c := Control{}
            c.Engine = engine
            c.Kind = kind
            c.Control = value
            c.Name = control
            e.Controls[control] = c
          }
          context[engine] = e
        }
        return context
}

func getControlContext(engine string, control string) Control {
	context := Control{}
        controls, _ := db.GetControls(engine)
        engines, _ := db.GetEngines()
        context.Kind = engines[engine]
        context.Control = controls[control]
        context.Engine = engine
        context.Name = control
        return context
}

func init() {
	var err error
	templates, err = template.New("dummy").Funcs(functions).ParseGlob(filepath.Join("templates", "*.gohtml"))
	if err != nil {
		fmt.Printf("\nError creating templates %v", err)
	}

	if templates != nil {
		http.HandleFunc("/", serveIndexTemplate)
		http.HandleFunc("/indexbody", serveIndexBodyTemplate)
	        http.HandleFunc("/port/{port}",servePortIndexTemplate)
	        http.HandleFunc("/port/{port}/indexbody",servePortIndexBodyTemplate)
                http.HandleFunc("/control",serveControlTemplate)
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
	err := templates.ExecuteTemplate(w, "indexpage", getFullContext())
	if err != nil {
		fmt.Printf("\nError executing index template %v", err)
	}
}

func serveIndexBodyTemplate(w http.ResponseWriter, r *http.Request) {
	err := templates.ExecuteTemplate(w, "indexbody", getFullContext())
	if err != nil {
		fmt.Printf("\nError executing indexbody template %v", err)
	}
}

func servePortIndexTemplate(w http.ResponseWriter, r *http.Request) {
	port := r.PathValue("port")
	err := templates.ExecuteTemplate(w, "portindexpage", port)
	if err != nil {
		fmt.Printf("\nError executing port index template %v", err)
	}
}

func servePortIndexBodyTemplate(w http.ResponseWriter, r *http.Request) {
	port := r.PathValue("port")
	err := templates.ExecuteTemplate(w, "portindexbody", port)
	if err != nil {
		fmt.Printf("\nError executing port indexbody template %v", err)
	}
}

func serveControlTemplate(w http.ResponseWriter, r *http.Request) {
       err := r.ParseForm()
        if err != nil {
                log.Printf("control service: parse form error %v", err)
                return
        }
        engine := r.Form.Get("engine")
        control := r.Form.Get("control")
        value := r.Form.Get("level")
        evtype := r.Form.Get("evtype")
        fmt.Printf("\n%s  SET engine %s control %s = %s",evtype,engine,control,value)
        err = db.UpdateControl(engine, control, value)
/*
	err = templates.ExecuteTemplate(w, "control", getControlContext(engine,control))
	if err != nil {
		fmt.Printf("\nError executing control template %v", err)
	}
*/
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
        fmt.Printf("\nMIDI %v to port %s",mididata,port)
        midi.Send(port,mididata)
}

