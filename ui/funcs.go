package main

import (
	"html/template"
	"time"
	"strconv"
)

var functions template.FuncMap

func init() {
	functions = template.FuncMap{
		"location":      time.LoadLocation,
		"unixmilli":     time.UnixMilli,
		"stepsize":      stepsize,
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
