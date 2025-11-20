#include <LiquidCrystal.h>
#include <IRremote.h>


int moisture_level = 0;
int soil_reading;


//// MAX AND MIN SENSOR READING ////
// MOISTURE
int min_soil = 30;
int max_soil = 80;

// TEMPERATURE
int min_temp = 55;
int max_temp = 90;

// HUMIDITY
int min_humid = 20;
int max_humid = 99;


bool status = false; // Controls Setting or Current

// SENSORS DISPLAY 
char* sensors[] = {"Soil", "Temp", "Humid"};
int current_sensor = 0;

// CURRENT SENSOR FUNCTIONS
void decrementCurrentSensor(){
  if(getCurrentSensorIndex() == 0) endCurrentSensor();
  else current_sensor -= 1;
}
void incrementCurrentSensor(){
	if(getCurrentSensorIndex() == 2) startCurrentSensor();
  	else current_sensor += 1;
}
int getCurrentSensorIndex(){
  return current_sensor;
}
void startCurrentSensor(){
  current_sensor = 0;
}
void endCurrentSensor(){
  current_sensor = 2;
}



//// OUPUTS VARIABLES ////
// DEVICES
const int moisture_sensor = A0;
const int water_pump = 2;
const int ir_receiver = 4;

//LCD
const int RS = 8;
const int E = 9;
const int DB4 = 10;
const int DB5 = 11;
const int DB6 = 12;
const int DB7 = 13;

LiquidCrystal lcd(RS, E, DB4, DB5, DB6, DB7);



//// IR RECEIVER CLASS ////

class Remote{
  public:
  void command(){}
};

class MoveLeft : public Remote {
  public:
  void command() {
  	incrementCurrentSensor();
  }
};
class MoverRight : public Remote {
  public:
  void command(){
    decrementCurrentSensor();
  }
};

// IR RECIEVER
struct RemoteCommand{
  long code;
  const char symbol;
  Remote obj;
};
RemoteCommand remote_code[] = {
  {4077698816, '0'},
  {4010852096, '1'},
  {3994140416, '2'},
  {3977428736, '3'},
  {3944005376, '4'},
  {3927293696, '5'},
  {3910582016, '6'},
  {3877158656, '7'},
  {3860446976, '8'},
  {3843735296, '9'},
  {4211392256, 'L', Remote action = MoverRight},
  {4177968896, 'R'},
  {4194680576, 'S'},
  {4244815616, 'W'}
};
// IR RECEIVER FUNCTION
char IRControls(){
  for(int i = 0; i < 14; i++){
    if(remote_code[i].code == IrReceiver.decodedIRData.decodedRawData){
      return remote_code[i].symbol;
    }
  }
  return 'E';
}


static const char* decodeStatus(bool status){
  if(!status) return "Cur";
  return "Set";
}
Remote remote;

void setup()
{
  Serial.begin(9500);
  
  // SETTING IN/OUT-PUT
  pinMode(moisture_sensor, INPUT);
  pinMode(water_pump, OUTPUT);
  
  // LCD SETTINGS
  lcd.begin(16,2);
  lcd.setCursor(0,1);
  lcd.print('<');
  lcd.setCursor(15,1);
  lcd.print('>');
  
  // IR RECIEVER SETTINGS
  IrReceiver.begin(ir_receiver);
  Remote remote;
  
  
}

void loop()
{
	display();
  
  	runPump(moisture_level);
  
   
  
  if(IrReceiver.decode() > 0){
    char command = IRControls();
  }

  delay(750);
}


//// FUNCTIONS ////

// WATERING SYSTEM FUNCTIONS
void runPump(int moisture){
  if(moisture < min_soil){ 
    digitalWrite(water_pump, HIGH);
  }
  else if(moisture > max_soil){
   	digitalWrite(water_pump, LOW); 
  }
}

// LCD FUNCITONS
void display(){
  displayStatus();
  displaySensor();
  displayReading();
}
void setDisplay(char letter){
  
}

void displayStatus(){
  lcd.setCursor(2,0);
  Serial.print(decodeStatus(status));
  lcd.print(decodeStatus(status));
  lcd.print(": ");
}
void displaySensor(){
  lcd.setCursor(6,0);
  lcd.print(getCurrentSensor());
}
void displayReading(){
  lcd.print(" ");
  lcd.print(getCurrentReading());
  lcd.print("      ");
}

// READING FUNCTIONS

int getTemp(){
  return 0;
}
int getHumid(){
  return 0;
}
int getSoil(){
  	soil_reading = analogRead(moisture_sensor);
  	moisture_level = map(soil_reading, 0, 876, 0, 100);
  	return moisture_level;
}
      
// SENSORS DISPLAY FUNCTIONS
char* getCurrentSensor(){
  return sensors[current_sensor];
}
int getCurrentReading(){
  switch(current_sensor){
    case 0: return getSoil();
  		break;
    case 1: return getTemp();
    	break;
    case 2: return getHumid();
    	break;
    default: Serial.println("ERROR: getting readings on getCuurrentSensor()");
    	break;
  }
}
