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

type ProgramRecord struct {
        Program int
        Name    string
}


func GetBanks(port string) []int {
	var banks = make([]int,0)

	row, err := database.Query("SELECT DISTINCT bank from patches where port = ?", port)
	if err != nil {
		return banks
	}
	defer row.Close()
	for row.Next() { // Iterate and fetch the records from result cursor
		var bank int
		row.Scan(&bank)
		banks = append(banks,bank)
	}
	return banks
}

func GetPrograms(port string, bank int) []ProgramRecord {
	var programs = make([]ProgramRecord,0)

	row, err := database.Query("SELECT program,name from patches where port = ? and bank = ?", port,bank)
	if err != nil {
		return programs
	}
	defer row.Close()
	for row.Next() { // Iterate and fetch the records from result cursor
		program := ProgramRecord{}
		row.Scan(&program.Program, &program.Name)
		programs = append(programs,program)
	}
	return programs
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
