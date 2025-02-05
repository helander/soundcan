package parameter

import "log"
import "sync"

var mutex sync.Mutex

type parameterSet map[string]string

var parameterSets map[string]parameterSet = make(map[string]parameterSet)

var selected string = ""

var parameters parameterSet = make(parameterSet)

func ParamValue(parameterName string, defaultValue string) string {
        mutex.Lock()
        defer mutex.Unlock()
	value, exists := parameters[parameterName]
	log.Printf("ParamValue: %s %s %s",parameterName,defaultValue,value)
	if exists {return value}
	return defaultValue
}

func Assign(parameterName string, value string) {
        mutex.Lock()
        defer mutex.Unlock()
	parameters[parameterName] = value
}

func Activate(parameterset string) {
	params, exists := parameterSets[parameterset]
	if exists {
		parameters = params
	} else {
		parameters = make(parameterSet)
	}
	selected = parameterset
}

func Store(parameterset string) {
	parameterSets[parameterset] = parameters
}

func Selected() string {
	return selected
}
