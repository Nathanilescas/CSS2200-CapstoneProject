#include <LiquidCrystal.h>
#include <IRremote.h>


class SensorSpecs {
    private:
        int min_soil = 30, max_soil = 80;
        int min_temp = 55, max_temp = 90;
        int min_humid = 20, max_humid = 99;

    public:
        // GETTERS
        int getMinSoil() { return min_soil; }
        int getMaxSoil() { return max_soil; }

        int getMinTemp() { return min_temp; }
        int getMaxTemp() { return max_temp; }

        int getMinHumid() { return min_humid; }
        int getMaxHumid() { return max_humid; }

        // SETTERS
        void setMinSoil(int min) { if(min > 0 && min < 100) min_soil = min; }
        void setMaxSoil(int max) { if(max > 0 && max < 100) max_soil = max; }

        void setMinTemp(int min) { if(min > 45 && min < 95) min_temp = min; }
        void setMaxTemp(int max) { if(max > 45 && max < 95) max_temp = max; }

        void setMinHumid(int min) { if(min > 0 && min < 100) min_humid = min; }
        void setMaxHumid(int max) { if(max > 0 && max < 100) max_humid = max; }
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
  {4244815616, 'W'} // SETUP-MODE
};

// DEVICES //
const int moisture_sensor = A0;
const int temp_sensor = A1;
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
            int temp_reading = analogRead(temp_sensor);
            int temp_level = map(temp_reading, 20, 358, -40, 125);
            return celToFah(temp_level);
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
    private:
    int celToFah(int cels) {
        return ((cels * 9) / 5) + 32;	
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
    }
    }

    void setSpecValues() {
        int join_num1 = joinDigits(specValues[0], specValues[1]);
        int join_num2 = joinDigits(specValues[2], specValues[3]);
      
        switch(current_sensor) {
            case 0: {
                specs.setMinSoil(join_num1);
              	specs.setMaxSoil(join_num2);
            }
                break;
            case 1: {
                specs.setMinTemp(join_num1);
                specs.setMaxTemp(join_num2);
            }
                break;
            case 2: {
                specs.setMinHumid(join_num1);
                specs.setMaxHumid(join_num2);
            }
                break;
            default: Serial.println("ERROR: switch(current_sensor");
                break;
        }
    }

    int joinDigits(int tens, int ones) {
        return (tens * 10) + ones;
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
            
            bool isTemp = (current_sensor == 1);
            if(isTemp) {
                lcd.print((char) 178); 
                lcd.print("F");
            }
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
    Serial.begin(9600);    
    lcd.begin(16,2);

    // IN/OUT-PUTS
    pinMode(moisture_sensor, INPUT);
    pinMode(temp_sensor, INPUT);
    pinMode(water_pump, OUTPUT);

    // IR RECEIVER
    IrReceiver.begin(ir_receiver);
    screen.display();    
}

void loop() {
    ir.receive();
}

//! LAST THING WORK ON REFRESH RATE