#include <string.h>
#include <stdlib.h>

#include <Keypad.h>

// ============
// SENSOR - KEY
// ============
// 1 - temperature
// 2 - light intensity
// 3 - motion
// 4 - distance
// 5 - tilt
// 6 - gas sensor


typedef struct sensor_t {
  float (*get) (void);
  char paramName[20];
  char unitName[10];
  int upperLimit;
  int lowerLimit;
} sensorType;

enum {SENSOR_TEMP, SENSOR_LDR, SENSOR_PIR, SENSOR_ULTRASONIC, SENSOR_TILT, SENSOR_GAS, SENSOR_NR_OF};


// ===================
// SENSOR LIMIT VALUES
// ===================
#define MAX_TEMP_LIMIT 100
#define MIN_TEMP_LIMIT 0
#define MAX_DISTANCE_LIMIT 200
#define MIN_DISTANCE_LIMIT 50
#define MAX_LIGHT_LIMIT 75
#define MIN_LIGHT_LIMIT 25
#define MAX_GAS_LIMIT 900
#define MIN_GAS_LIMIT 720


// ==========
// TMP SENSOR
// ==========

#define TEMPERATURE_TMP36_PIN A1
#define ADC_MIN 0
#define ADC_MAX 1023
#define TEMP_MIN -40
#define TEMP_MAX 125
#define VOLT_MIN 0
#define VOLT_MAX 5

void setupTemperature()
{
 	pinMode(TEMPERATURE_TMP36_PIN, INPUT);
}

float getTemperature()
{
  int rawData = analogRead(TEMPERATURE_TMP36_PIN);
  float celsius = map(((rawData-20)*3.04), ADC_MIN, ADC_MAX, TEMP_MIN, TEMP_MAX);

  return celsius;
}

// ==========
// LDR sensor
// ==========
#define LDR_PIN A0
#define LIGHT_MIN 10
#define LIGHT_MAX 100

void setupLDR()
{
 	pinMode(LDR_PIN, INPUT); 
}

float getLight()
{
  int rawData = analogRead(LDR_PIN);
  float voltage = map(rawData, ADC_MIN, ADC_MAX, VOLT_MIN, VOLT_MAX),
  			light = map(voltage, VOLT_MIN, VOLT_MAX, LIGHT_MIN, LIGHT_MAX);
  
  return light;
}

// ==========
// PIR sensor
// ==========
#define PIR_PIN 13

void setupPIR()
{
	pinMode(PIR_PIN, INPUT); 
}

float getPIRData()
{
	return digitalRead(PIR_PIN); 
}

// =================
// Ultrasonic sensor
// =================
#define TRIG_PIN 12
#define ECHO_PIN 11

void setupUltrasonic()
{
		pinMode(TRIG_PIN, OUTPUT);
  	pinMode(ECHO_PIN, INPUT);
}

float getDistance()
{
	long distance, duration;
  
  	digitalWrite(TRIG_PIN, LOW);
  	delayMicroseconds(2);
  
  	digitalWrite(TRIG_PIN, HIGH);
  	delayMicroseconds(10);
  	digitalWrite(TRIG_PIN, LOW);
  	
  	duration = pulseIn(ECHO_PIN, HIGH);
  	distance = duration * 0.034 / 2;
  
  	return distance;
}


// ===================
// SW 200D Tilt sensor
// ===================
#define TILT_PIN 10

void setupTilt()
{
	pinMode(TILT_PIN, INPUT_PULLUP);
  	//attachInterrup(1, blink, RISING);
}

float getTilt()
{
	 if(digitalRead(TILT_PIN) == HIGH)
       	return 1;
  	 return 0;
}

// ================
// Gas sensor (MQ6)
// ================
#define GAS_PIN A2

void setupGas()
{
	pinMode(GAS_PIN, INPUT); 
}

float getGas()
{
	return analogRead(GAS_PIN); 
}


// ====
// LEDS
// ====
#define GREEN_LED_PIN A4
#define RED_LED_PIN A5

void setupLEDs()
{
	pinMode(GREEN_LED_PIN, OUTPUT);
 	pinMode(RED_LED_PIN, OUTPUT);
}

void led(unsigned int ledPin, unsigned int delayTime)
{
  digitalWrite(ledPin, HIGH);
  delay(delayTime);
  digitalWrite(ledPin, LOW);
}


// ===========
// Sensor List
// ===========

sensorType sensorList[SENSOR_NR_OF] = {
  {
 	getTemperature,
  	"temperature",
  	"celsius",
    MAX_TEMP_LIMIT,
    MIN_TEMP_LIMIT
  },
  {
  	getLight,
    "light intensity",
    "points",
	MAX_LIGHT_LIMIT,
    MIN_LIGHT_LIMIT
  },
  {
  	getPIRData,
    "motion",
    "yes/no",
  },
  {
  	getDistance,
    "distance",
    "cm",
    MAX_DISTANCE_LIMIT,
    MIN_DISTANCE_LIMIT
  },
  {
  	getTilt,
    "tilt",
    "yes/no"
  },
  {
   	getGas,
    "gas",
    "ppm",
    MAX_GAS_LIMIT,
    MIN_GAS_LIMIT
  }
};

float getSensorData(int id)
{
  if(id >= 0 && id < SENSOR_NR_OF)
  {
   	return sensorList[id].get(); 
  }
}

char* getSensorParamName(int id)
{
  if(id >= 0 && id < SENSOR_NR_OF)
  {
   	return sensorList[id].paramName; 
  }
}

char* getSensorUnit(int id)
{
  if(id >= 0 && id < SENSOR_NR_OF)
  {
   	return sensorList[id].unitName; 
  }
}


void printSensorData(float value, char* param, char* unit)
{
  Serial.print(param);
  Serial.print(": ");
  Serial.print(String(value));
  Serial.print(" ");
  Serial.print(unit);
  Serial.println(", ");
}

void displaySensorData(unsigned int id)
{
  	if(id < 0 || id >= SENSOR_NR_OF) return;
  
 	char *paramName = getSensorParamName(id),
    	 *unitName = getSensorUnit(id);
    float value = getSensorData(id);
    
    // pentru valori boolean - ex: PIR (yes/no)
    if(strcmp(unitName, "yes/no") == 0)
    {
     		Serial.print(paramName);
      	Serial.print(": ");
      
      	if(value == 1)
          Serial.println("yes,");
      	else Serial.println("no,");
    }
    else {
      	// pentru neincadrarea in limita     
      	int upperLimit = sensorList[id].upperLimit,
      			lowerLimit = sensorList[id].lowerLimit;
      
      	if(value > upperLimit) led(GREEN_LED_PIN, 100);
      	if(value < lowerLimit) led(RED_LED_PIN, 100);
      
    	printSensorData(value, paramName, unitName);
    } 
}


// ======
// Keypad
// ======
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4

char hexaKeys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[KEYPAD_ROWS] = {9, 8, 7, 6},
	 	 colPins[KEYPAD_COLS] = {5, 4, 3, 2};

Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS );

void listenKeypad()
{
	char key = customKeypad.getKey();
  
  	if(key != NO_KEY)
      	// switch in loc de conversie din char in int
      	// pentru ca exista butoane ca 'A'sau '#' si nu va
      	// corespunde indicelui din enumerare
    	switch (key)
        {
        		case '1':  	displaySensorData(0); break;
          	case '2':		displaySensorData(1); break;
          	case '3': 	displaySensorData(2); break;
          	case '4': 	displaySensorData(3); break;
      			case '5':   displaySensorData(4); break;
      			case '6':		displaySensorData(5); break;
          	default: 	Serial.println("No such sensor!"); break;
        }
}


// ======
//  Main
// ======

void setup()
{
  Serial.begin(9600);
  setupTemperature();
  setupLDR();
  setupPIR();
  setupUltrasonic();
  setupTilt();
  setupGas();
  setupLEDs();
}


void loop()
{
  listenKeypad();

  delay(50);
}
