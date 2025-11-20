#include <LiquidCrystal.h>
#include <IRremote.h>



// GLOBAL VARIABLES
char* sensors[3] = {"Soil", "Temp", "Humid"};
int current_sensor = 0;
int ui_state = false;

struct RemoteControl {
  long code;
  const char symbol;
};

RemoteControl remote_definitions[] = {
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
  {4211392256, 'L'}, // LEFT
  {4177968896, 'R'}, // RIGHT
  {4194680576, 'S'}, // SELECT
  {4244815616, 'W'} // SETUP-MODE
};

// DEVICES //
const int moisture_sensor = A0;
const int water_pump = 2;
const int ir_receiver = 4;

const int RS = 8;
const int E = 9;
const int DB4 = 10;
const int DB5 = 11;
const int DB6 = 12;
const int DB7 = 13;
LiquidCrystal lcd(RS, E, DB4, DB5, DB6, DB7);

// CLASSES 
class SensorSpecs {
    private:
        int min_soil = 30;
        int max_soil = 80;

        int min_temp = 55;
        int max_temp = 90;

        int min_humid = 20;
        int max_humid = 99;
    public:
        // GETTERS
        int getMinSoil() {
            return min_soil;
        }
        int getMaxSoil() {
            return max_soil;
        }

        int getMinTemp() {
            return min_temp;
        }
        int getMaxTemp() {
            return max_temp;
        }

        int getMinHumid() {
            return min_humid;
        }
        int getMaxHumid() {
            return max_humid;
        }

        // SETTERS
        void setMinSoil(int min) {
            if(min > 0 && min < 100) min_soil = min;
        }
        void setMaxSoil(int max) {
            if(max > 0 && max < 100) max_soil = max;
        }

        void setMinTemp(int min) {
            if(min > 45 && min < 95) min_soil = min;
        }
        void setMaxTemp(int max) {
            if(max > 45 && max < 95) max_soil = max;
        }

        void setMinHumid(int min) {
            if(min > 0 && min < 100) min_soil = min;
        }
        void setMaxHumid(int max) {
            if(max > 0 && max < 100) max_soil = max;
        }
};


class SensorReader{
    public: 
        int getTemp() {

        }
        int getHumid() {

        }
        int getSoil() {
            int soil_reading = analogRead(moisture_sensor);
            int moisture_level = map(soil_reading, 0, 876, 0, 100);
            return moisture_level;
        }
};

class Command {
  public: 
  // NOTE THESE ARE ALL SETTING FUNCTION THAT DOESN'T WORRY ABOUT THE DISPLAY, ITS LIKE A SUPPORTING CLASS FOR DISPLAY
  void shiftViewLeft() {
    _decrementSensor();
  }
  void shiftViewRight() {
    _incrementSensor();
  }
	
  void setUpMode() {
  	if(_getUIState()) _disableUI();
    else _enableUI();
  }
   void enterNumber(int num){
    // USE NUM TO FIND THE NEXT SPOT FOR THE CURSOR AND PRINT NUM
  	// NOTE: THIS IS ONLY TO BE USED IF SETUP IS ENABLED
     if(_getUIState()){
       
     }
  }
  void select() {
    // SAVES THE CHAR WHERE THE CURSOR IS OVER BEFORE IT MOVES ON
  }
  
  private:
  const int num_sensors = 3;
  

  
  void _incrementSensor() {
    int index = (current_sensor + 1) % num_sensors;
    current_sensor = index;
  }
  void _decrementSensor() {
    if(current_sensor == 0) current_sensor = 2;
    else current_sensor -= 1;
  }
  
  // GETTERS/SETTERS
  void _disableUI() { ui_state = false; }
  void _enableUI() { ui_state = true; }
  bool _getUIState() { return ui_state; }
};

class Remote {
    public:
        void receive() {
            if(IrReceiver.decode() > 0) {
               char command = _decodeInput();
               if(command != -1) {
                    _runCommands(command);
               } else {
                // diplay.ERROR(); DISPLAY AN ERROR OCCURED ON THE LCD
               }
            }

            IrReceiver.resume();
        }

    private:
        char _decodeInput() {
            for(int i = 0; i < 14; i++) {
                if(remote_definitions[i].code == IrReceiver.decodedIRData.decodedRawData) {
                    return remote_definitions[i].symbol;
                }
            }
          	Serial.println("_decodeInput could not find code in input list");
            return -1;
        }
        void _runCommands(char code) {
          	Command cmd;
            switch(code) {
                case 'L': cmd.shiftViewLeft();
                    break;
                case 'R': cmd.shiftViewRight();
                    break;
                case 'S': cmd.select();
              		break;
              	case 'W': cmd.setUpMode();
                	break;
              	default: cmd.enterNumber(code);
            };
        }
};

class Display {
  
    public:
        void display(int value) {
          
		  _navArrows();
        }
        void setUp() {
			
        }
    private:
        SensorReader _source;

        void _navArrows() {
            lcd.setCursor(0,1);
            lcd.print('<');
            lcd.setCursor(15,1);
            lcd.print('>');
        }
};

// STARTER VARIABLES
Remote remote;

void setup() {
    Serial.begin(9500);    
    lcd.begin(16,2);

    // IN/OUT-PUTS
    pinMode(moisture_sensor, INPUT);
    pinMode(water_pump, OUTPUT);

    // IR RECEIVER
    IrReceiver.begin(ir_receiver);
    Serial.print(current_sensor);
    
}

void loop() {
    remote.receive();
  	delay(750);
}