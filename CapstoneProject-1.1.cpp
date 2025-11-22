#include <LiquidCrystal.h>
#include <IRremote.h>


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

// GLOBAL VARIABLES
char* sensors[3] = {"Soil", "Temp", "Humid"};
int current_sensor = 0;
int ui_state = false;
SensorSpecs specs;

int specValues[4] = {-1, -1, -1, -1};
void clearMM() {
    for(int i = 0; i < 4; i++) {
        specValues[i] = -1;
    }
}

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
class SensorReader{
    public: 
        int getTemp() {
            // don't forget to connect the sensor and finish this method
            return 50;
        }
        int getHumid() {
            // don't forget to connect the sensor and finish this method
            return 60;
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
  	if(_getUIState()) {
      _disableUI();
      clearMM();   
    }
    else _enableUI();
  }
  void enterNumber(int num) {
    if(_getUIState()){
        for(int i = 0; i < 4; i++) {
            if(specValues[i] < 0) {
                specValues[i] = num;
                return;
            }
        }
		setSpecValues();
        setUpMode();
        // display the default display

 
    }
    }
    void setSpecValues() {
      char x = specValues[0] + '0';
      char y = specValues[1] + '0';
      int convert_num1 = x + y - '0';
      
      x = specValues[2] + '0';
      y = specValues[] + '0';
      int convert_num2 = (x + y) - '0';
      
      Serial.println(convert_num1);
      Serial.println(convert_num2);
        switch(current_sensor) {
            case 0: {
                specs.setMinSoil(convert_num1);
              	specs.setMaxSoil(convert_num2);
            }
                break;
            case 1: {

            }
                break;
            case 2: {

            }
                break;
            default: 
                break;
        }
    }
    int concatenateNumber(int num1, int num2) {
		return 0;
    }


    //   void select() {
    //     // SAVES THE CHAR WHERE THE CURSOR IS OVER BEFORE IT MOVES ON
    //   }
  
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

class Display {
  
    public:
        void display() {
            if(ui_state) {
                _displaySensor();
                _displaySpecs();
            } else {
              _displaySensor();
              _displayLiveValue();
              _navArrows();
            }
        }
    private:
        SensorReader _source;

        void _navArrows() {
            lcd.setCursor(0,1);
            lcd.print('<');
            lcd.setCursor(15,1);
            lcd.print('>');
        }
        void _displaySensor() {
            char* sensor_name = sensors[current_sensor];
            lcd.setCursor(4,0);
            lcd.print(sensor_name);
            lcd.print(": ");
        }
        void _displayLiveValue() {
            lcd.print(_getLiveSensorValue(current_sensor));
        }


        void _displaySpecs() {
            _displayMin();
            _displayMax();
            _displaySensorValues();
        }
        void _displayMin() {
            lcd.setCursor(0, 1);
            lcd.print("Min: ");
        }
        void _displayMax() {
            lcd.setCursor(8, 1);
            lcd.print("Max: ");
        }
        void _displaySensorValues() {
            int min_placment = 5;
            int max_placement = 13;

            _printValue();
            
        }
        void _printValue() {
            lcd.setCursor(5, 1);
            for(int i = 0; i < 4; i++) {
                if(i == 2)lcd.setCursor(13, 1);
                if(specValues[i] > 0) {
                    lcd.print(specValues[i]);
                }else {
                    lcd.blink();
                  	return;
                }
            }
        }

        int _getLiveSensorValue(int index) {
            switch (index) {
                case 0: return _source.getSoil();
                    break;
                case 1: return _source.getTemp();
                    break;
                case 2: return _source.getHumid(); 
                    break;
                default: {
                    Serial.println("ERROR AT _getCurrentSensor");
                    return -1;
                }
                    break;
            }
        }
};

class Remote {
    Display tempScreen;
    public:
        void receive() {
            if(IrReceiver.decode() > 0) {
               lcd.clear();
               char command = _decodeInput();
                Serial.println(command);
               if(command != -1) {
                    _runCommands(command);
                 tempScreen.display();
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
              	case 'W': cmd.setUpMode();
                	break;
              	default: {
                  	int convertedInt = code - '0';
                	cmd.enterNumber(convertedInt);
                }
            };  
        }
};



// STARTER VARIABLES
Remote ir;
Display screen;

void setup() {
    Serial.begin(9500);    
    lcd.begin(16,2);

    // IN/OUT-PUTS
    pinMode(moisture_sensor, INPUT);
    pinMode(water_pump, OUTPUT);

    // IR RECEIVER
    IrReceiver.begin(ir_receiver);
    screen.display();    
}

void loop() {
    ir.receive();
  	delay(750);
}