package main

import (
//	"net"
	"net/http"
//	"strings"
//	"time"
)


func main() {
	http.Handle("/", http.FileServer(http.Dir("/usr/share/webui/www")))
	http.ListenAndServe(":9999", nil)
}

