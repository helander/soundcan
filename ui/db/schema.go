package db

var schema []string = []string{
`CREATE TABLE IF NOT EXISTS engines ( engine TEXT NOT NULL, kind TEXT, PRIMARY KEY (engine) ) WITHOUT ROWID;`,
`CREATE TABLE IF NOT EXISTS controls ( engine TEXT NOT NULL, control TEXT NOT NULL, value TEXT, min TEXT, max TEXT,  PRIMARY KEY (engine, control) ) WITHOUT ROWID;`,
`CREATE TABLE IF NOT EXISTS valuemaps ( engine TEXT NOT NULL, name TEXT NOT NULL, key TEXT NOT NULL, value TEXT,  PRIMARY KEY (engine, name, key) ) WITHOUT ROWID;`,
}
