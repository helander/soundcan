package main

import "log"
//import "time"

import "github.com/helander/soundcan/galv/jalv"

func main(){
	log.Printf("BEFORE =============================================")
	jalv.JalvMain()
	log.Printf("AFTER  =============================================")
}

