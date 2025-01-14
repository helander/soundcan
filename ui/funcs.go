package main

import (
	"html/template"
	"time"
	"strconv"

	"github.com/helander/soundcan/ui/db"
	"github.com/helander/soundcan/ui/midi"
)

var functions template.FuncMap

func init() {
	functions = template.FuncMap{
		"location":      time.LoadLocation,
		"unixmilli":     time.UnixMilli,
		"stepsize":      stepsize,
		"banks":	banks,
		"programs":	programs,
		"bank": midi.SelectedBank,
		"program": midi.SelectedProgram,
	}
}

func stepsize(min string, max string) float64 {
   if min == "0" && max == "1" {
		return 1.0
   }
   fMin, _ := strconv.ParseFloat(min,64)
   fMax, _ := strconv.ParseFloat(max,64)
   return (fMax - fMin)/100.0
}


func banks(port string) []int {
	return db.GetBanks(port)
}

func programs(port string) []db.ProgramRecord {
	return db.GetPrograms(port, midi.SelectedBank(port))
}
