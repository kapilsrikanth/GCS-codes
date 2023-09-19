#include <Arduino.h>
#include <Printers.h>
#include <XBee.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial (9,10);

#define MAX_FRAME_DATA_SIZE 110
String packet_count;
String Team_ID = "C";
String MISSION_time;
String MODE;

String state;
String alt;
String airSpeed;
String HS_DEP;
String PC_DEP;
String temp;
String volt;
String GPS_time;
String GPS_alt;
String GPS_long;
String GPS_lat;
String GPS_sats;
String TiltX;
String tilty;
String rotZ;
String Command;
String CMD_ECHO;
String telemetryValues [20];
bool allowReading;
bool calibrateAltitude;
XBee xbee;

XBeeResponse response;

ZBRxResponse rx = ZBRxResponse();
ZBTxRequest tx;
XBeeAddress64 addr64 = XBeeAddress64(0x0013A200, 0x12345678);
uint32_t highAddress = 0x0013A200;
uint32_t lowAddress = 0x12345678;

void setup() {
  // put your setup code here, to run once:
   Serial.begin(9600);
    mySerial.begin(9600);
    xbee.setSerial(mySerial);
    XBeeAddress64(highAddress,lowAddress);

    

}
 
void loop() {
  read_packet();
  cal_alt();
  processCommand();
  BCN();
  SIM_Enable();
  SIM_Activate();
  simping_for_pressure();
  
  delay(100);

}
void read_packet() {
    allowReading = false;
    if(Serial.available()>0){
      Command = Serial.readStringUntil('\n');
      if (Command.equals("CMD,TEAM C,CX,ON")){
        allowReading = true;
        Serial.println("Start Reading");
      }
    }
    while (allowReading == true) {
      Serial.println(".");
      if(Serial.available()>0){
        Command = Serial.readStringUntil('\n');
        if (Command.equals("CMD,TEAM C,CX,OFF")){
          allowReading = false;
          Serial.println("Stop Reading");
        }
      }
      else if (mySerial.available()>0){
        xbee.readPacket();
        if (xbee.getResponse().isAvailable()) {
            if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
              xbee.getResponse().getRx16Response(rx);
              for (int i = 0; i < rx.getDataLength(); i++) {
                telemetryValues[i] += static_cast<char>(rx.getData()[i]);                    
              }
              telemetryValues[0] =  Team_ID;
              
              telemetryValues[1] =  MISSION_time;
              telemetryValues[2] =  packet_count;
              telemetryValues[3] =  MODE;
              telemetryValues[4] =  state;
              telemetryValues[5] =  alt;
              telemetryValues[6] =  airSpeed;
              telemetryValues[7] =  HS_DEP;
              telemetryValues[8] =  PC_DEP;
              telemetryValues[9] =  temp;
              telemetryValues[10] = volt;
              telemetryValues[11] = GPS_time;
              telemetryValues[12] = GPS_alt;
              telemetryValues[13] = GPS_long;
              telemetryValues[14] = GPS_lat;
              telemetryValues[15] = GPS_sats;
              telemetryValues[16] = TiltX;
              telemetryValues[17] = tilty;
              telemetryValues[18] = rotZ;
              telemetryValues[19] = CMD_ECHO;
            }
          }
      }
      //Serial.print(Team_ID + ", " + MISSION_time + ", " + packet_count + ". " + MODE + ", " + state + ", " + alt + ", " + ", " + airSpeed + ", " + HS_DEP + ", " + PC_DEP + ", " + temp + ", " + volt + ", " + GPS_time + ", " + GPS_alt + ", " + GPS_long + ", " + GPS_lat + ", " + GPS_sats + ", " + TiltX + ", " + tilty + ", " + rotZ + ", " + CMD_ECHO);
      delay(100);
    }
}



void cal_alt() {
 
  
  if (Serial.available()>0) {
    Command = Serial.readStringUntil('\n');
    if (Command.equals("CMD,TEAM C,CAL")){
      Serial.println("Calibrating Altitude");
       
       
       uint8_t data_to_be_sent[] = {"Calibrate Altitude to Zero"};
       
       tx.setPayload(data_to_be_sent, sizeof(data_to_be_sent));
       tx= ZBTxRequest(addr64, data_to_be_sent, sizeof(data_to_be_sent));
       xbee.send(tx);   

    }
  }
  
}
void processCommand() {
  if(Serial.available()>0){
    Command = Serial.readStringUntil('\n');
String    expectedCommand = "CMD,Team C,ST";
    
  
  if (Command.startsWith(expectedCommand)) {
   
    String timePart = Command.substring(expectedCommand.length());
    
    if (timePart.equals("GPS")) {
      
      getTimeFromGPS();
    } else {
      
      setMissionTime(timePart);
    }
  }
}
}

void getTimeFromGPS() {
 uint8_t set_gps_time[] = {"CMD,C,ST,GPS"};
       
       tx.setPayload(set_gps_time, sizeof(set_gps_time));
       tx= ZBTxRequest(addr64, set_gps_time, sizeof(set_gps_time));
       xbee.send(tx);  
  
}

void setMissionTime(String time) {
 uint8_t set_utc_time[] = {"CMD,C,ST,UTC"};

       
       tx.setPayload(set_utc_time, sizeof(set_utc_time));
       tx= ZBTxRequest(addr64, set_utc_time, sizeof(set_utc_time));
       xbee.send(tx);
  
}

void BCN() {
  if(Serial.available()>0){
    Command = Serial.readStringUntil('\n');
    if(Command.equals("CMD,TEAM C,BCN,ON")){
      uint8_t audio = {"BCN ON"};

      tx.setPayload(audio,sizeof(audio));
      tx= ZBTxRequest(addr64, audio, sizeof(audio));
      xbee.send(tx);
    }
  }
}

void SIM_Enable() {
  if(Serial.available()>0){
    Command = Serial.readStringUntil('\n');
    if(Command.equals("CMD,TEAM C,SIM, Enable")){
      uint8_t enable = {"Enable Simulation"};

      tx.setPayload(enable,sizeof(enable));
      tx= ZBTxRequest(addr64,enable, sizeof(enable));
      xbee.send(tx);
    }
  }
}

void SIM_Activate() {
  if(Serial.available()>0){
    Command = Serial.readStringUntil('\n');
    if(Command.equals("CMD,TEAM C,SIM,ACTIVATE")){
      uint8_t activate= {"Enable Activation"};

      tx.setPayload(activate,sizeof(activate));
      tx= ZBTxRequest(addr64,activate,sizeof(activate));
      xbee.send(tx);
    }
  }
}
void simping_for_pressure() {
  if(Serial.available()>0){
    Command = Serial.readStringUntil('\n');
    String expectedCommand = "CMD,Team C,SIMP";
    if(Command.startsWith(expectedCommand)){
      String subCommand = Command.substring(expectedCommand.length());
      int arrayLength = subCommand.length();
      uint8_t sim_pressure1[arrayLength];
      if (arrayLength > 0 && arrayLength <= sizeof(sim_pressure1)) {
        for (int i = 0; i < arrayLength; i++) {
        sim_pressure1[i] = static_cast<uint8_t>(subCommand.charAt(i));
    }
      } else {
   
}



      tx.setPayload(sim_pressure1,sizeof(sim_pressure1));
      tx= ZBTxRequest(addr64,sim_pressure1,sizeof(sim_pressure1));
      xbee.send(tx);
      
    }
      
  }
}
