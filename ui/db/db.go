package db

import (
	"database/sql"
	"fmt"
	gosqlite "github.com/glebarez/go-sqlite"
//	sqlitelib "modernc.org/sqlite/lib"
)

var database *sql.DB

type  ControlValue struct {
  Value string
  Min string
  Max string
}

func GetEngines() (map[string]string, error) {
	var engines = make(map[string]string)

	row, err := database.Query("SELECT engine,kind from engines")
	if err != nil {
		return engines, err
	}
	defer row.Close()
	for row.Next() { // Iterate and fetch the records from result cursor
		var engine string
		var kind string
		row.Scan(&engine, &kind)
		engines[engine] = kind
	}
	return engines, nil
}

func GetControls(engine string) (map[string]ControlValue, error) {
	var controls = make(map[string]ControlValue)

	row, err := database.Query("SELECT control,value,min,max from controls where engine = ?", engine)
	if err != nil {
		return controls, err
	}
	defer row.Close()
	for row.Next() { // Iterate and fetch the records from result cursor
		var control string
		var value string
		var min string
		var max string
		row.Scan(&control, &value, &min, &max)
		controls[control] = ControlValue{value, min, max}
                //fmt.Printf("\n[%s] %v",control,controls[control])
	}
	return controls, nil
}

func UpdateControl(engine string, control string, value string) error {
	_, err := database.Exec("UPDATE controls SET value = ? WHERE engine = ? and control = ?", value, engine, control)
	return err
}



func init() {

	dbFilePath := "/session/settings.db"

	fmt.Printf("\nCreate/open database file %s", dbFilePath)
	var err error
	database, err = sql.Open("sqlite", dbFilePath)
	if err != nil {
		fmt.Printf("\nError opening database file %v", err.(*gosqlite.Error))
		return
	}

	//defer db.Close()
    for _, source := range schema {
      //fmt.Printf("\nCreate from schema  %s",source)
      if _, err = database.Exec(source); err != nil {
		fmt.Printf("\nError creating from %s", source)
      }
    }

}
