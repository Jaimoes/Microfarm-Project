/* TODO:
    Add functions to control when to put on lights/fan
	  Edit Error handling function > LED green / yellow PULSE
    Check getNTP function > No connection = offline modus enabled after x tries > retry every x 
    Lightcontrol should have input for plant > how long time should be on off per plant
*/

/* Pins used:
  2 = DHT11 INSIDE
  3 = DHT11 OUTSIDE
  5 = DS18B20
  4 = SD
  6 = Pump
  7 = Lights
  8 = Fan
  10 to 13 = Ethernet

  44/23 = Indication LED
  45 = Fan speed
  
  A0 = Sharp IR
  A1 = Soil humidity
*/

/* Plant types 
	0 = 
	1 =
	

 */
#define PlantType 1

/* Ethernet */
#include <Ethernet.h>
#include <SPI.h>
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//IPAddress localServer = (192,168,1,74 ); // Server IP UNUSED
//byte localServer[] = { 192, 168, 1, 74 };
char dataServer[] = "greensworth.nl";
EthernetClient client;
IPAddress ip(192, 168, 1, 25);
bool connectedWithEthernet;
EthernetServer server(80);
/* DHT    */
#include <DHT.h>
#define DHTTYPE DHT11   // DHT 11
DHT dhtInside(2, DHTTYPE);
DHT dhtOutside(3, DHTTYPE);
/* DS18B20 */
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 5 // connected to pin #
#define TEMPERATURE_PRECISION 5
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress;
/* Relay pins */
int pumpPin = 6;
int lightPin = 7;
int fanPin = 8;
/* Sharp IR */
#include <SharpIR.h>
#define ir A0 // The pin
#define model 1080 // 1080 for GP2Y0A21Y
SharpIR sharp(ir, 500, 98, model);  //500 readings before avg / 98% close to last reading
/* Time */
#include <TimeLib.h>
#include <EthernetUdp.h>
const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets
EthernetUDP Udp;
unsigned int localPort = 8888; // local port to listen for UDP packets
const int timeZone = 2;     // Central European Time
IPAddress timeServer(132, 163, 4, 101); // time-a.timefreq.bldrdoc.gov
/* Pump Variables */
int pumpOnFor = 5;
/* Soil Humidity variables */
int soilHumPin = A1;  // Analog Pin
int minSoilHum = 450; // Add correct variable !!!!
int SoilHum = 0;
/* Multithread vars */
long previousMillis = 0;
long interval = 10000; // 300000 = 5minutes
/* Temperature control  */
int minTemp = 22;
int maxTemp = 27;
/* Error handling  */
int error = 0;
int ledOK = 44;
int ledBAD = 23;
/* Fan Control variables */
int fanControlPin = 45;
/* Plant Age */
#include <EEPROM.h>
String readString, newString;
int currentPlantAge = 0;
boolean plantAgeReset = false;
boolean plantAgeSet = false;




void setup() {
  Serial.begin(9600);
  Serial.println("Mircofarm software");
  Serial.println("Version v0.9");
  Serial.println("Setting up");
  Serial.println(".............................................");
  Serial.println();
  /* DHT */
  Serial.println("Starting DHT sensors, please wait...");
  dhtInside.begin();
  dhtOutside.begin();
  delay(2000);
  testDHTconnection(dhtOutside);
  testDHTconnection(dhtInside);
  /* Ethernet */
  Serial.println("Starting ethernet, please wait...");
  startEthernet();
  server.begin();
  /* DS18B20 */
  Serial.println("Starting DS18B20 sensor, please wait...");
  sensors.begin();
  testDS18B20();
  Serial.println("Done.");
  /* Setting output pins */
  setOutputPins();
  /* Time */
  Serial.println("Setting up time()...");
  Udp.begin(localPort);
  setSyncProvider(getNtpTime);
  /* PlantAge */
  checkPlantAgeInit();

  
  
  
  Serial.println("Setup done!");
}

void loop() {
  unsigned long currentMillis = millis();
  if (timeStatus() != timeNotSet) {
    time_t t = now();
	if(plantAgeSet){
		
		/* Control functions */
		updatePlantAge();
		errorHandling();
		fanControl(true, 1);
		plantLightControl(PlantType);
		microfarmWebInterface();

		if (currentMillis - previousMillis > interval) {
			soilHumControl();
      
			if ( hour() > 9 || hour() < 22 ){
				pumpControl();
			}
			//serverControl();
      

			previousMillis = currentMillis;
//   		Serial.print("Farm temperature is: ");
//     	Serial.println(getTemp(dhtInside));
//     	Serial.print("Farm humidity is: ");
//     	Serial.println(getHumd(dhtInside));
//     	Serial.print("Farm index is: ");
//     	Serial.println(getIndx(dhtInside));
//     	Serial.print("Growbed temperature is: ");
//     	Serial.println(getGroundTemp(tempDeviceAddress));
//     	Serial.println("");
//     	Serial.print("Outside temperature is: ");
//     	Serial.println(getTemp(dhtOutside));
//    	Serial.print("Outside humidity is: ");
//     	Serial.println(getHumd(dhtOutside));
//     	Serial.print("Outside index is: ");
//      Serial.println(getIndx(dhtOutside));
//      Serial.println("");
//			Serial.print("Soil humidity is: ");
//			Serial.print(getSoilHum());
//			Serial.println("%");
//			Serial.print("Soil humidity average is: ");
//			Serial.print(SoilHum);
//			Serial.println("%");
//			Serial.println("");
//			Serial.print("Time: ");
//			Serial.print(hour());
//			Serial.print(":");
//			Serial.println(minute());
//			Serial.println("");
//			Serial.print("Current day: ");
//			Serial.println(day());
			Serial.print("Plant age is currently ");
			Serial.println(currentPlantAge);
//			Serial.print("Plant age was set on date: ");
//			Serial.print(getDayPlantAgeWasSet());
//			Serial.print("/");
//			Serial.println(getMonthPlantAgeWasSet());
			sendClimateData2Server(getTemp(dhtOutside),getHumd(dhtOutside),getTemp(dhtInside), getHumd(dhtInside),getGroundTemp(tempDeviceAddress), getSoilHum());
	  
		}
	} else {
    Serial.println("Plant age was not set, please use the web interface to set plant age");
    Serial.print("Web interface can be found on: ");
    Serial.println(Ethernet.localIP());
		microfarmWebInterface();
		error = 1;
		errorHandling();
		if(plantAgeReset){
			plantAgeSet = true;
			error = 0;
		}
	}
    
	
    
  } else {
	  
	  Serial.println("Time was not set correctly or could not be retrieved.");
	  Serial.println("System will not start without time set.");
	  error = 1;
	  errorHandling();
	  delay(10000);
  } // ELSE RESET TIME!!!
}
void sendPlantAgeToServer(int plantAge){
	
	Serial.print("Connecting to server....");
	if (client.connect(dataServer, 80)) { 
		Serial.println("connected!");
		client.print("GET /addPlantAge.php?plantAge=");
		client.print(plantAge);
		client.println(" HTTP/1.1");
		client.print("Host: ");
		client.println(dataServer);
//    	client.println("192.168.1.74");
		client.println("Connection: close");
		client.println();
		delay(100);
		client.stop();
		Serial.println("Update send to server!"); 
	} else {
		Serial.println("pumpOn encountered an error!");
	}
}
void checkPlantAgeInit(){
	if(getDayPlantAgeWasSet() != 0 && getDayPlantAgeWasSet() != 255){
		if(getMonthPlantAgeWasSet() != 0 && getMonthPlantAgeWasSet() != 255){
			if(getPlantAge() != 0 && getPlantAge() != 255){
				plantAgeSet = true;
				currentPlantAge = getPlantAge(); 
				Serial.println("Plant age is set!");	
			}
		}
	} else {
		Serial.print("Plant age isn't set!");
	}
}
void writeMonthToEEPROM(int month){
	EEPROM.update(2, byte(month));
	Serial.print("Writing ");
	Serial.print(byte(month));
	Serial.println(" to EEPROM");
}
int getMonthPlantAgeWasSet(){
	byte i;
	EEPROM.get(2, i);
	return int(i);
}
void writeDayToEEPROM(int day){
	EEPROM.update(1, byte(day));
	Serial.print("Writing ");
	Serial.print(byte(day));
	Serial.println(" to EEPROM");
}
int getDayPlantAgeWasSet(){
	byte i;
	EEPROM.get(1, i);
	return int(i);
}
void writePlantAgeToEEPROM(int plantAge) {	
	EEPROM.update(0, byte(plantAge));
	Serial.print("Writing ");
	Serial.print(byte(plantAge));
	Serial.println(" to EEPROM");
	plantAgeReset = true;
}
int getPlantAge(){
	byte i;
	EEPROM.get(0, i);
	return int(i);
}
void updatePlantAgeOnEEPROM(int plantAge){
	EEPROM.update(0, byte(plantAge));
	plantAgeReset = true;
}
void microfarmWebInterface(){
	EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        //read char by char HTTP request
        if (readString.length() < 100) {

          //store characters to string 
          readString += c; 
          //Serial.print(c);
        } 

        //if HTTP request has ended
        if (c == '\n') {

          ///////////////
          Serial.print(readString); //see what was captured

          //now output HTML data header

          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();

          client.println("<HTML>");
          client.println("<HEAD>");
          client.println("<TITLE>Microfarm internal settings page</TITLE>");
          client.println("</HEAD>");
          client.println("<BODY>");

          client.println("<H1>Microfarm internal settings</H1>");

          client.println("<FORM ACTION='/' method=get >"); //uses IP/port of web page

          client.println("Set plant age in days: <INPUT TYPE=TEXT NAME='PLANT' VALUE='' SIZE='25' MAXLENGTH='25'><BR>");

          client.println("<INPUT TYPE=SUBMIT NAME='submit' VALUE='Submit plant age'>");

          client.println("</FORM>");

          client.println("<BR>");

          client.println("</BODY>");
          client.println("</HTML>");

          delay(1);
          //stopping client
          client.stop();

          /////////////////////
          if (readString.length() >0) {
            //Serial.println(readString); //prints string to serial port out
            int pos1 = readString.indexOf('=');
            int pos2 = readString.indexOf('&');
            newString = readString.substring(pos1+1, pos2);
            //Serial.print("newString is: ");
            //Serial.println(newString);
            int n = newString.toInt();
			
			if(n != 0){
				writePlantAgeToEEPROM(n);
			    writeDayToEEPROM(day());
			    writeMonthToEEPROM(month());
				sendPlantAgeToServer(n);
			}
			      
            Serial.print("The value sent is: ");
            Serial.println(n);
            readString=""; //clears variable for new input    
            newString=""; //clears variable for new input
            // auto select appropriate value
            
          }           
        }
      }
    }
  }
}
int plantAgeToWeeks(){
	return currentPlantAge / 7;
}
void plantLightControl(int plantID){
	
	switch(plantID){
		case 0:
		switch(plantAgeToWeeks()){
			case 0:
			lightControl(24);
			break;
			case 1:
			lightControl(24);
			break;
			case 2:
			lightControl(24);
			break;
			case 3:
			lightControl(18);
			break;
			case 4:
			lightControl(18);
			break;
			case 5:
			lightControl(18);
			break;
			case 6:
			lightControl(18);
			break;
			case 7:
			lightControl(16);
			break;
			case 8:
			lightControl(16);
			break;
		}		
		break;
		case 1:
		switch(plantAgeToWeeks()){
			case 0:
			lightControl(24);
			break;
			case 1:
			lightControl(24);
			break;
			case 2:
			lightControl(24);
			break;
			case 3:
			lightControl(18);
			break;
			case 4:
			lightControl(18);
			break;
			case 5:
			lightControl(18);
			break;
			case 6:
			lightControl(18);
			break;
			case 7:
			lightControl(16);
			break;
			case 8:
			lightControl(16);
			break;
		}		
		break;
		default:
		switch(plantAgeToWeeks()){
			case 0:
			lightControl(24);
			break;
			case 1:
			lightControl(24);
			break;
			case 2:
			lightControl(24);
			break;
			case 3:
			lightControl(18);
			break;
			case 4:
			lightControl(18);
			break;
			case 5:
			lightControl(18);
			break;
			case 6:
			lightControl(18);
			break;
			case 7:
			lightControl(16);
			break;
			case 8:
			lightControl(16);
			break;
		}
		break;
		
	}
}
void updatePlantAge(){
	static int paMonth = getMonthPlantAgeWasSet();
	static int paDay = getDayPlantAgeWasSet();
	static int paOriginal = getPlantAge();
	
	
	
	//Check if EEPROM version of plantAge has been reset since init.
	if(plantAgeReset){
		currentPlantAge = getPlantAge();
		plantAgeReset = false;
	}
	//Check if day changed since plantAge was set > if so update.
	if(month() == paMonth){
		if(day() > paDay){
			int dayDifference = day() - paDay;
			int paDifference = currentPlantAge - paOriginal;
			if(dayDifference > paDifference){
        Serial.println("Updating plant age...");
				currentPlantAge += (dayDifference - paDifference);
				//updatePlantAgeOnEEPROM(currentPlantAge);
			}
			  
		}
	}
	if(month() != paMonth){
		int monthValues[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
		int monDifference = month() - paMonth;
		int paMonthInDays;
		if(monDifference >= 1){
			for(int i = paMonth; i < (month()); i++){
				paMonthInDays += monthValues[(i - 1)];
			}
			paMonthInDays -= paDay;
			paMonthInDays += day();
		} else if(monDifference < 0){
			for(int i = paMonth; i < 12 + 1; i++){
				paMonthInDays += monthValues[(i - 1)];
			}
			for(int i = 1; i < month() + 1; i++){
				paMonthInDays += monthValues[i];
			}
			paMonthInDays -= paDay;
			paMonthInDays += day();
		}
		if(currentPlantAge != paMonthInDays){
			currentPlantAge = paMonthInDays;
			Serial.print("currentPlantAge = ");
			Serial.println(currentPlantAge);      
		}
		paMonthInDays = 0;
	}
}
void lightControl(int hoursDay){
	
	int startTime = 8;
	int timeOff = startTime	+ hoursDay;
	boolean over24 = false;
	
	if (timeOff > 24) {
		timeOff -= 24;
		over24 = true;
    //Serial.println("Over 24!");
    
	}
	//Serial.println(hour(t));
	switch(over24) {
		case 0:
      //Serial.println(" under ");
			if ( hour() >= startTime && hour() < timeOff ) {
				lightsOn(true);
			} 
			if ( hour() >= timeOff ) {
				lightsOn(false);
			}
			if ( hour() < startTime ) {
				lightsOn(false);
			}
		break;
		case 1:
			if ( hour() >= timeOff && hour() < startTime ){
       lightsOn(false);
      } else {
        lightsOn(true);
      }
		break;
	}
	
}
void pumpOn2Server(){
	Serial.print("Connecting to server....");
  if (client.connect(dataServer, 80)) { 
    Serial.println("connected!");
    client.print("GET /updateIrrigationTime.php?isOn=");
    client.print(1);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(dataServer);
//    client.println("192.168.1.74");
    client.println("Connection: close");
    client.println();
    delay(100);
    client.stop();
    Serial.println("Update send to server!"); 
  } else {
    Serial.println("pumpOn encountered an error!");
  }
}
void serverControl(){
	static boolean doOnce;
  
	if(minute() == 30 || minute() == 0){
    if(!doOnce){
      sendClimateData2Server(getTemp(dhtOutside),getHumd(dhtOutside),getTemp(dhtInside), getHumd(dhtInside),getGroundTemp(tempDeviceAddress), getSoilHum());
      doOnce = true;
    }
	} else if ( minute() != 30 || minute() != 0) {
		doOnce = false;
	}
}
void lightsOn(boolean onOff){
  if(onOff){
    digitalWrite(lightPin, HIGH);
  } else {
    digitalWrite(lightPin, LOW);
  }
}
void fanControl(boolean onOff, int percentSpeed){
	if (onOff) {
		digitalWrite(fanPin, HIGH);
		setFanSpeed(percentSpeed);
	} else {
		digitalWrite(fanPin, LOW);
	}
}
void fanControl(boolean onOff){
	if (onOff) {
		digitalWrite(fanPin, HIGH);
		setFanSpeed(50);
	} else {
		digitalWrite(fanPin, LOW);
	}
}
void setFanSpeed(int percentSpeed){
	
	int speedVar = map(percentSpeed, 0, 100, 0, 1023);
	
	analogWrite(fanControlPin, speedVar);
	
}
void errorHandling() {
  static int pulseVar = 0;
  static boolean upDown = false;

  if ( error == 0) {
    if (upDown) {
      pulseVar--;
    } 
    if (!upDown) {
      pulseVar++;
    } 
    if (pulseVar == 255){
      upDown = true;
    } 
    if(pulseVar == 0){
      upDown = false;
    }
    delay(30);
    analogWrite(ledOK, pulseVar);
    digitalWrite(ledBAD, LOW);
  } else if (error == 1) {
    digitalWrite(ledBAD, HIGH);
    analogWrite(ledOK, 255);
  }
}
void pumpControl() { // turn pump on if soil hum is low
  static int pumpTimeOn;
  static boolean doOnce = false;

  if ( SoilHum != 0 && SoilHum >= minSoilHum ) {
    if (!doOnce) {
      pumpTimeOn = second() + pumpOnFor;
      doOnce = true;
	  pumpOn2Server();
    }
//    Serial.print("pumpTimeOn ");
//    Serial.println(pumpTimeOn);
    if ( pumpTimeOn > second()) {
      pumpOn(true);
    } else {
      SoilHum = 0;
      doOnce = false;
    }
  } else {
    pumpOn(false);
  }
}
void soilHumControl() {
  static int soilHumReads[10];
  static int Next_soilHumControl;
  static int soilHumAvg;
  int totalSoilHum = 0;
  boolean soilHumReadsFull = false;

  if (minute() >= Next_soilHumControl || Next_soilHumControl == 0) {

    for (int i = 0; i < (sizeof(soilHumReads) / sizeof(int)); i++) {

      if (soilHumReads[i] == 0) {
        soilHumReads[i] = getSoilHum();
        break;
      }
      if (i == ((sizeof(soilHumReads) / sizeof(int)) - 1)) {
        soilHumReadsFull = true;
        break;
      }
//      Serial.print("i: ");
//      Serial.print(i);
//      Serial.print(" = ");
//      Serial.println(soilHumReads[i]);
    }

    Next_soilHumControl = minute() + 1;
    if (Next_soilHumControl > 59) {
      Next_soilHumControl -= 60;
    }
//    Serial.print("Next_soilHumControl = ");
//    Serial.println(Next_soilHumControl);
  }
  if (soilHumReadsFull) {
    for (int i = 0; i < (sizeof(soilHumReads) / sizeof(int)); i++) {
      totalSoilHum += soilHumReads[i];
      soilHumReads[i] = 0;
    }
    soilHumReadsFull = false;
    SoilHum = totalSoilHum / (sizeof(soilHumReads) / sizeof(int));
  }

}
void pumpOn( boolean on ) {
  if (on) {
    digitalWrite(pumpPin, HIGH);
  } else {
    digitalWrite(pumpPin, LOW);
  }
}
int getSoilHum() { // Get data from sensor

  int soilHum;

  soilHum = analogRead(soilHumPin);

  if (soilHum == 0 || soilHum < 0 || soilHum > 1024) {
    error = 1;
    return 999;
  } else {
    return map(soilHum, 0, 1023, 100, 0);
  }
}
void sendClimateData2Server(float tempOutside, float humOutside, float tempInside, float humInside, float tempSoil, float humSoil) {
  Serial.print("Connecting to server....");
  if (client.connect(dataServer, 80)) { 
    Serial.println("connected!");
    client.print("GET /addClimateData.php?tO=");
    client.print(tempOutside);
    client.print("&hO=");
    client.print(humOutside);
    client.print("&tI=");
    client.print(tempInside);
    client.print("&hI=");
    client.print(humInside);
    client.print("&tS=");
    client.print(tempSoil);
	client.print("&hS=");
    client.print(humSoil);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(dataServer);
//    client.println("192.168.1.74 ");
    client.println("Connection: close");
    client.println();
    delay(100);
    client.stop();
    Serial.println("Data send to server!"); 
  } else {
    Serial.println("sendData encountered an error!");
  }
}
int plantHeight() {
  return sharp.distance() - 45; // 45 = IR to growbed in CM
}
bool checkDHToutput(float input) {
  if (isnan(input)) {
    return true;
  } else {
    return false;
  }
}
void testDHTconnection(DHT dht) {
  float t = dht.readTemperature();
  if ( checkDHToutput(t)) {
    Serial.println("DHT sensor has errors!");
	error = 1;
    Serial.println("Please verify connection before continuing.");
  } else {
    Serial.println("DHT sensor started.");
  }
}
void startEthernet() {
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Serial.println("Trying to configure Ethernet with static IP: 192.168.1.55 ");
    Ethernet.begin(mac, ip);
  } else {
    Serial.print("Ethernet started on: ");
    Serial.println(Ethernet.localIP());
    connectedWithEthernet = true;
  }
}
int renewEthernet() {
  Serial.println("Renewing Ethernet lease.......");
  switch (Ethernet.maintain()) {
    case 0:
      Serial.println("Nothing happened.");
	  return 0;
      break;
    case 1:
      Serial.println("Renew failed.");
	  return 1;
      break;
    case 2:
      Serial.println("Renew success.");
	  return 2;
      break;
    case 3:
      Serial.println("Rebind fail.");
	  return 3;
      break;
    case 4:
      Serial.println("Rebind success.");
	  return 4;
      break;
  }
}
float getTemp(DHT dht) {
  float t = dht.readTemperature();
  if (checkDHToutput(t)) {
    // TODO if error, what to do
	error = 1;
  } else {
    return t;
  }
}
float getHumd(DHT dht) {
  float h = dht.readHumidity();
  if (checkDHToutput(h)) {
    // TODO if error, what to do
	error = 1;
  } else {
    return h;
  }
}
float getIndx(DHT dht) {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (checkDHToutput(h) || checkDHToutput(t)) {
    // TODO if error, what to do
	error = 1;
  } else {
    return dht.computeHeatIndex(t, h, false);;
  }
}
float getGroundTemp(DeviceAddress deviceAddress) {
  sensors.requestTemperatures();
  sensors.getTempC(deviceAddress);
}
void testDS18B20() {
  int numberOfDevices;
  numberOfDevices = sensors.getDeviceCount();
  if (numberOfDevices == 0) {
    Serial.print("System found ");
    Serial.print(numberOfDevices);
    Serial.println(" devices");
    error = 1;
  }

  for (int i = 0; i < numberOfDevices; i++)
  {
    // Search the wire for address
    if (sensors.getAddress(tempDeviceAddress, i))
    {
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: ");
      printAddress(tempDeviceAddress);
      Serial.println();

      Serial.print("Setting resolution to ");
      Serial.println(TEMPERATURE_PRECISION, DEC);

      // set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
      sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);

      Serial.print("Resolution actually set to: ");
      Serial.print(sensors.getResolution(tempDeviceAddress), DEC);
      Serial.println();
    } else {
      Serial.print("Found ghost device at ");
      Serial.print(i, DEC);
      Serial.print(" but could not detect address. Check power and cabling");
    }
  }
}
void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
void setOutputPins() {
  Serial.println("Setting output pins....");
  pinMode(pumpPin, OUTPUT);
  pinMode(lightPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  pinMode(ledOK, OUTPUT);
  pinMode(ledBAD, OUTPUT);
  pinMode(fanControlPin, OUTPUT);
  Serial.println("Done.");
}

void sendNTPpacket(IPAddress & address) { // send an NTP request to the time server at the given address
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
time_t getNtpTime() {
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  if(renewEthernet() == 1 || renewEthernet() == 3) {
    startEthernet();
  }
  return 0; // return 0 if unable to get the time
}



v