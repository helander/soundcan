package fluidsynth

import (
	"html/template"
	"log"
	"strings"

)

var Functions template.FuncMap

func init() {
	Functions = template.FuncMap{
		"fonts": Fonts,
		"channelfont": ChannelFont,
		"fluidengine": GetEngine,
		"fluidchannel": FluidChannel,
	}
}

type ChannelRecord struct {
	Host	string
	Port	string
	Channel string
	Sfont	string
	Bank	string
	Preset	string
	Volume	string
}

type InstrumentRecord struct {
	Bank	string
	Preset	string
	Name	string
}

type FontRecord struct {
	Host	string
	Port	string
	Font	string
	Filename string
	Instruments []InstrumentRecord
}

type EngineKey struct {
	Host	string
	Port	string
}

type EngineRecord struct {
	Host	string
	Port	string
	Fonts	map[string]FontRecord
	Channels map[string]ChannelRecord
}

var engines	map[EngineKey]EngineRecord = make(map[EngineKey]EngineRecord)

func FluidChannel(host string, port string, channel string) map[string]any {
	mChannel := make(map[string]any)
	mChannel["host"] = host
	mChannel["port"] = port
	rEngine := getEngine(host,port)
	rChannel, exists := rEngine.Channels[channel]
	if !exists {
		rChannel = ChannelRecord{}
		rChannel.Channel = channel
		rChannel.Sfont = "1"
		rChannel.Bank = "0"
		rChannel.Preset = "0"
		rChannel.Volume = "0"
	        FluidsynthCommand(host, port, "cc "+rChannel.Channel+" "+"7"+" "+rChannel.Volume)
	        FluidsynthCommand(host, port, "select "+rChannel.Channel+" "+rChannel.Sfont+" "+rChannel.Bank+" "+rChannel.Preset)
		rEngine.Channels[channel] = rChannel
	}
	mChannel["channel"] = rChannel.Channel
	mChannel["volume"] = rChannel.Volume
	mChannel["font"] = rChannel.Sfont
	mChannel["bank"] = rChannel.Bank
	mChannel["preset"] = rChannel.Preset
	return mChannel
}

func getEngine(host string, port string) EngineRecord {
	key := EngineKey{}
	key.Host = host
	key.Port = port
	engine, _ := engines[key]
	return engine
}

func GetEngine(host string, port string) EngineRecord {
	key := EngineKey{}
	key.Host = host
	key.Port = port
	engine, keyExists := engines[key]
	if keyExists { return engine }
	engine = EngineRecord{}
	engine.Host = host
	engine.Port = port

	response, err := FluidsynthCommand(host,port,"fonts")
	if err != nil {
		log.Printf("fluidprogram error %v",err)
	}

	fonts := make(map[string]FontRecord)
	rows := strings.Split(string(response),"\n")
	for _,row := range rows[1:len(rows)-1] {
		columns := strings.Fields(row)
		font := FontRecord{}
		font.Host = host
		font.Port = port
		font.Font = columns[0]
		filename := columns[1]
		font.Filename = filename[strings.LastIndex(filename, "/")+1:]
		fonts[columns[0]] = font
	}

	for key,_ := range fonts {
		response, err = FluidsynthCommand(host,port,"inst "+key)
		if err != nil {
			log.Printf("fluidprogram error %v",err)
		}
		rows = strings.Split(string(response),"\n")
		instruments := make([]InstrumentRecord,0)
		for _,row := range rows[:len(rows)-1] {
			bank := row[0:3]
			preset := row[4:7]
			name := row[8:]
			instrument := InstrumentRecord{}
			instrument.Bank = bank
			instrument.Preset = preset
			instrument.Name = name
			instruments = append(instruments, instrument)
		}
		font := fonts[key]
		font.Instruments = instruments
		fonts[key] = font
	}
	engine.Fonts = fonts

	channels := make(map[string]ChannelRecord)
	response, err = FluidsynthCommand(host,port,"channels -verbose")
	if err != nil {
		log.Printf("fluidprogram error %v",err)
	}
	rows = strings.Split(string(response),"\n")
	for _,row := range rows[:len(rows)-1] {
		columns := strings.Split(row,",")
		channel := ChannelRecord{}
		channel.Host = host
		channel.Port = port
		for _, column := range columns {
			fields := strings.Fields(column)
			switch fields[0] {
				case "chan":
					channel.Channel = fields[1]
				case "sfont":
					channel.Sfont = fields[1]
				case "bank":
					channel.Bank = fields[1]
				case "preset":
					channel.Preset = fields[1]
			}
		}
		channel.Volume = "0"
	        FluidsynthCommand(host, port, "cc "+channel.Channel+" "+"7"+" "+channel.Volume)
		channels[channel.Channel] = channel
	}
	engine.Channels = channels
	engines[key] = engine

	// Unison using fluidsynth router

//	FluidsynthCommand(host, port, "router_clear")
/*
	FluidsynthCommand(host, port, "router_begin note")
	FluidsynthCommand(host, port, "router_chan 0 7 0 0")
	FluidsynthCommand(host, port, "router_end")
	FluidsynthCommand(host, port, "router_begin note")
	FluidsynthCommand(host, port, "router_chan 0 7 0 1")
	FluidsynthCommand(host, port, "router_end")
	FluidsynthCommand(host, port, "router_begin note")
	FluidsynthCommand(host, port, "router_chan 0 7 0 2")
	FluidsynthCommand(host, port, "router_end")
	FluidsynthCommand(host, port, "router_begin note")
	FluidsynthCommand(host, port, "router_chan 0 7 0 3")
	FluidsynthCommand(host, port, "router_end")
	FluidsynthCommand(host, port, "router_begin note")
	FluidsynthCommand(host, port, "router_chan 0 7 0 4")
	FluidsynthCommand(host, port, "router_end")
	FluidsynthCommand(host, port, "router_begin note")
	FluidsynthCommand(host, port, "router_chan 0 7 0 5")
	FluidsynthCommand(host, port, "router_end")
	FluidsynthCommand(host, port, "router_begin note")
	FluidsynthCommand(host, port, "router_chan 0 7 0 6")
	FluidsynthCommand(host, port, "router_end")
	FluidsynthCommand(host, port, "router_begin note")
	FluidsynthCommand(host, port, "router_chan 0 7 0 7")
	FluidsynthCommand(host, port, "router_end")
*/
	return engine
}

func ChannelFont(channel ChannelRecord) FontRecord {
	engineKey := EngineKey{}
	engineKey.Host = channel.Host
	engineKey.Port = channel.Port
	engine := engines[engineKey]
	return engine.Fonts[channel.Sfont]
}

func Fonts(host string, port string) map[string]FontRecord {
	engineKey := EngineKey{}
	engineKey.Host = host
	engineKey.Port = port
	engine := engines[engineKey]
	return engine.Fonts
}
