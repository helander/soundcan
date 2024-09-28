#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

enum ValueType {
    TYPE_BOOL,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    END_OF_LIST
};

typedef struct CONTROL_T control_t;

struct CONTROL_T {
    enum ValueType type;
    char *label;
    char *minValue;
    char *maxValue;
    char *points;
    char readValue[30];
    char writeValue[30];
    void (*readFunc)(control_t *);
    void (*writeFunc)(control_t *);
};

extern void runCommand(char *command, char *response, int responseMax);

static int nmbControls;

static char *typeImage(enum ValueType type)
{
  switch(type) {
    case TYPE_BOOL: return "boolean";
    case TYPE_INT: return "integer";
    case TYPE_FLOAT: return "float";
    case TYPE_STRING: return "string";
  }
  return NULL;
}

static void readTitle(control_t *control)
{
  char response[100];
  runCommand("get midi.jack.id", response, sizeof(response));
  char *token = strtok(response,"\n");
  strcpy(control->readValue,token);
}

static void writeTitle(control_t *control)
{
}

static void readGain(control_t *control)
{
  char response[100];
  runCommand("get synth.gain", response, sizeof(response));
  char *token = strtok(response,"\n");
  strcpy(control->readValue,token);
}

static void writeGain(control_t *control)
{
  char command[50];
  sprintf(command,"set synth.gain %.3f",atof(control->writeValue));
  char response[100];
  runCommand(command, response, sizeof(response));
}


static void readInstrument(control_t *control)
{
  int bank = -1;
  int preset = -1;
  char response[1000];

  runCommand("channels -verbose", response, sizeof(response));

  const char row_delimiters[] = "\n";

  const char column_delimiters[] = " ,";
  char* row_saveptr = NULL;
  char* column_saveptr = NULL;
  char* row_token = strtok_r(response, row_delimiters, &row_saveptr);
  // Change if to while to loop thru all channels
  if (row_token != NULL) {
     char* column_token = strtok_r(row_token, column_delimiters, &column_saveptr);
     while (column_token != NULL) {
        if (strcmp(column_token,"bank") == 0) {
           char *next = strtok_r(NULL, column_delimiters,&column_saveptr);
           bank = atoi(next);
        } else if (strcmp(column_token,"preset") == 0) {
           char *next = strtok_r(NULL, column_delimiters,&column_saveptr);
           preset = atoi(next);
        }
        column_token = strtok_r(NULL, column_delimiters,&column_saveptr);                       
     }
     row_token = strtok_r(NULL, row_delimiters,&row_saveptr);
  }
  sprintf(control->readValue,"%03d-%03d",bank,preset);
}

static void writeInstrument(control_t *control)
{
  int bank;
  int preset;

  char *sBank = strtok(control->writeValue,"-");
  if (sBank != NULL) {
    char *sPreset = strtok(NULL,"-");
    if (sPreset != NULL) {
       bank = atoi(sBank);
       preset = atoi(sPreset);
       char command[50];
       sprintf(command,"select 0 1 %d %d",bank,preset);
       char response[100];
       runCommand(command, response, sizeof(response));
    }
  }
}


static void readReverbActive(control_t *control)
{
  char response[100];
  runCommand("get synth.reverb.active", response, sizeof(response));
  char *token = strtok(response,"\n");
  strcpy(control->readValue,token);
}


static void writeReverbActive(control_t *control)
{
  char command[50];
  sprintf(command,"set synth.reverb.active %d",atoi(control->writeValue));
  char response[100];
  runCommand(command, response, sizeof(response));
}


static void readReverbDamp(control_t *control)
{
  char response[100];
  runCommand("get synth.reverb.damp", response, sizeof(response));
  char *token = strtok(response,"\n");
  strcpy(control->readValue,token);
}


static void writeReverbDamp(control_t *control)
{
  char command[50];
  sprintf(command,"set synth.reverb.damp %.3f",atof(control->writeValue));
  char response[100];
  runCommand(command, response, sizeof(response));
}


static void readReverbLevel(control_t *control)
{
  char response[100];
  runCommand("get synth.reverb.level", response, sizeof(response));
  char *token = strtok(response,"\n");
  strcpy(control->readValue,token);
}


static void writeReverbLevel(control_t *control)
{
  char command[50];
  sprintf(command,"set synth.reverb.level %.3f",atof(control->writeValue));
  char response[100];
  runCommand(command, response, sizeof(response));
}


static void readReverbRoomSize(control_t *control)
{
  char response[100];
  runCommand("get synth.reverb.room-size", response, sizeof(response));
  char *token = strtok(response,"\n");
  strcpy(control->readValue,token);
}


static void writeReverbRoomSize(control_t *control)
{
  char command[50];
  sprintf(command,"set synth.reverb.room-size %.3f",atof(control->writeValue));
  char response[100];
  runCommand(command, response, sizeof(response));
}


static void readReverbWidth(control_t *control)
{
  char response[100];
  runCommand("get synth.reverb.width", response, sizeof(response));
  char *token = strtok(response,"\n");
  strcpy(control->readValue,token);
}


static void writeReverbWidth(control_t *control)
{
  char command[50];
  sprintf(command,"set synth.reverb.width %.3f",atof(control->writeValue));
  char response[100];
  runCommand(command, response, sizeof(response));
}


static void readChorusActive(control_t *control)
{
  char response[100];
  runCommand("get synth.chorus.active", response, sizeof(response));
  char *token = strtok(response,"\n");
  strcpy(control->readValue,token);
}


static void writeChorusActive(control_t *control)
{
  char command[50];
  sprintf(command,"set synth.chorus.active %d",atoi(control->writeValue));
  char response[100];
  runCommand(command, response, sizeof(response));
}


static void readChorusDepth(control_t *control)
{
  char response[100];
  runCommand("get synth.chorus.depth", response, sizeof(response));
  char *token = strtok(response,"\n");
  strcpy(control->readValue,token);
}


static void writeChorusDepth(control_t *control)
{
  char command[50];
  sprintf(command,"set synth.chorus.depth %.3f",atof(control->writeValue));
  char response[100];
  runCommand(command, response, sizeof(response));
}


static void readChorusLevel(control_t *control)
{
  char response[100];
  runCommand("get synth.chorus.level", response, sizeof(response));
  char *token = strtok(response,"\n");
  strcpy(control->readValue,token);
}


static void writeChorusLevel(control_t *control)
{
  char command[50];
  sprintf(command,"set synth.chorus.level %.3f",atof(control->writeValue));
  char response[100];
  runCommand(command, response, sizeof(response));
}


static void readChorusNr(control_t *control)
{
  char response[100];
  runCommand("get synth.chorus.nr", response, sizeof(response));
  char *token = strtok(response,"\n");
  strcpy(control->readValue,token);
}


static void writeChorusNr(control_t *control)
{
  char command[50];
  sprintf(command,"set synth.chorus.nr %d",atoi(control->writeValue));
  char response[100];
  runCommand(command, response, sizeof(response));
}


static void readChorusSpeed(control_t *control)
{
  char response[100];
  runCommand("get synth.chorus.speed", response, sizeof(response));
  char *token = strtok(response,"\n");
  strcpy(control->readValue,token);
}


static void writeChorusSpeed(control_t *control)
{
  char command[50];
  sprintf(command,"set synth.chorus.speed %.3f",atof(control->writeValue));
  char response[100];
  runCommand(command, response, sizeof(response));
}



control_t controls[] = {
  { TYPE_STRING, "Title", NULL, NULL, NULL, "", "", readTitle, writeTitle },
  { TYPE_FLOAT, "Gain", "0.0", "1.0", NULL, "", "", readGain, writeGain },
  { TYPE_STRING, "Instrument", NULL, NULL, NULL, "", "", readInstrument, writeInstrument }, //Index hardcoded in initControl
  { TYPE_INT, "Reverb active", "0", "1", NULL, "", "", readReverbActive, writeReverbActive },
  { TYPE_FLOAT, "Reverb damp", "0.0", "1.0", NULL, "", "", readReverbDamp, writeReverbDamp },
  { TYPE_FLOAT, "Reverb level", "0.0", "1.0", NULL, "", "", readReverbLevel, writeReverbLevel },
  { TYPE_FLOAT, "Reverb room-size", "0.0", "1.0", NULL, "", "", readReverbRoomSize, writeReverbRoomSize },
  { TYPE_FLOAT, "Reverb width", "0.0", "100.0", NULL, "", "", readReverbWidth, writeReverbWidth },
  { TYPE_INT, "Chorus active", "0", "1", NULL, "", "", readChorusActive, writeChorusActive },
  { TYPE_FLOAT, "Chorus depth", "4.0", "10.0", NULL, "", "", readChorusDepth, writeChorusDepth },
  { TYPE_FLOAT, "Chorus level", "0.25", "2.5", NULL, "", "", readChorusLevel, writeChorusLevel },
  { TYPE_INT, "Chorus nr", "0", "10", NULL, "", "", readChorusNr, writeChorusNr },
  { TYPE_FLOAT, "Chorus speed", "0.1", "5.0", NULL, "", "", readChorusSpeed, writeChorusSpeed },
  { END_OF_LIST },
}; 

void getControlInstance(int index, char *response, void *context)
{
  if (index >= nmbControls) return;
  control_t *control = &controls[index];
  control->readFunc(control);
  strcpy(response,control->readValue);
}

void setControlInstance(int index, char *value, void *context)
{
  if (index >= nmbControls) return;
  control_t *control = &controls[index];
  strcpy(control->writeValue,value);
  control->writeFunc(control);
}

static char instrumentPoints[20000];

void initControl(void *context)
{
  for(nmbControls = 0; controls[nmbControls].type != END_OF_LIST; nmbControls++) {} 

  char response[5000];
  runCommand("inst 1", response, sizeof(response));
  strcpy(instrumentPoints,"[");
  char *sInstrument = strtok(response,"\n");
  bool comma = false;
  while(sInstrument != NULL) {
    if (comma) strcat(instrumentPoints,",");
    sInstrument[7] = '\0';
    char record[200];
    sprintf(record,"{\"value\":\"%s\",\"symbol\":\"%s\"}",sInstrument,sInstrument+8);
    strcat(instrumentPoints,record);
    comma = true;
    sInstrument = strtok(NULL,"\n");
  }
  strcat(instrumentPoints,"]");
  controls[2].points = instrumentPoints; // Hard coded index
}

void getControlInstances(char *response, void *context)
{
  initControl(context);
  strcpy(response,"[");
  for(int i = 0; controls[i].type != END_OF_LIST; i++) {
    control_t *control = &controls[i];
    control->readFunc(control);
    if (i==0) strcat(response,"{"); else strcat(response,",{");
    char record[20000];
    sprintf(record,"\"index\":%d",i);strcat(response,record);
    sprintf(record,",\"type\":\"%s\"",typeImage(control->type));strcat(response,record);
    sprintf(record,",\"label\":\"%s\"",control->label);strcat(response,record);
    if (control->minValue != NULL) {sprintf(record,",\"min\":\"%s\"",control->minValue);strcat(response,record);}
    if (control->maxValue != NULL) {sprintf(record,",\"max\":\"%s\"",control->maxValue);strcat(response,record);}
    sprintf(record,",\"value\":\"%s\"",control->readValue);strcat(response,record);
    if (control->points != NULL) {sprintf(record,",\"points\":%s",control->points);strcat(response,record);}
    strcat(response,"}");
  }
  strcat(response,"]");
}

