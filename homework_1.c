// Smocvin Denis
// IOT Bootcamp
// 01.06.2020

#define ADC_MIN 0
#define ADC_MAX 1023
#define VOLTAGE_MIN 0
#define VOLTAGE_MAX 5

// LDR
#define LDR_PIN A0
#define LIGHT_MIN 10
#define LIGHT_MAX 100

#define LIGHT_MAX_LIMIT 30

void setupLDR()
{
	pinMode(LDR_PIN, INPUT); 
}

float getLight()
{
	float voltage = map(analogRead(LDR_PIN), ADC_MIN, ADC_MAX, VOLTAGE_MIN, VOLTAGE_MAX); 
  
  return map(voltage, VOLTAGE_MIN, VOLTAGE_MAX, LIGHT_MIN, LIGHT_MAX);
}

// Relay
#define RELAY_PIN 13
#define RELAY_ON_DELAY 3000

void setupRelay()
{
	pinMode(RELAY_PIN, OUTPUT); 
}

void relayOn()
{
	digitalWrite(RELAY_PIN, HIGH); 
}

void relayOff()
{
	digitalWrite(RELAY_PIN, LOW); 
}

// PIR
#define PIR_PIN 8

unsigned long delayStart = 0;

void setupPIR()
{
	pinMode(PIR_PIN, INPUT); 
}

int getMotion()
{
	return digitalRead(PIR_PIN); 
}

void setup()
{
	  Serial.begin(9600);
  	setupLDR();
  	setupPIR();
  	setupRelay();
}

void loop()
{
  	float light = getLight();
  	int motion = getMotion();
  
	Serial.println(light);
  	Serial.println(motion);
  
  if(light < LIGHT_MAX_LIMIT && motion == HIGH) {
    relayOn();
    delay(RELAY_ON_DELAY);
  }
  else relayOff();
  
  delay(500);
}
