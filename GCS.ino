#include <Arduino.h>
#include <Printers.h>
#include <XBee.h>

#define MAX_FRAME_DATA_SIZE 110
String packet_count;
String Team_ID;
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

XBee xbee;

XBeeResponse response;

ZBRxResponse rx = ZBRxResponse();
ZBTxRequest tx;
XBeeAddress64 addr64 = XBeeAddress64(0x0013A200, 0x12345678);
uint32_t highAddress = 0x0013A200;
uint32_t lowAddress = 0x12345678;
bool allowReading = true;
void setup() {
  // put your setup code here, to run once:
   Serial.begin(115200);
    xbee.setSerial(Serial);
    XBeeAddress64(highAddress,lowAddress);

    

}
 
void loop() {
  if(Serial.available()){
    Command = Serial.readStringUntil('\n');
  
    read_packet();
    cal_alt();
    processCommand();
}
}
 



void read_packet(){
    
    
    if (allowReading) {
       Serial.println("Hi");
        xbee.readPacket();
        
        if (xbee.getResponse().isAvailable()) {
            if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
                xbee.getResponse().getRx16Response(rx);
                for (int i = 0; i < rx.getDataLength(); i++) {
                    telemetryValues[i]= rx.getData(i);
                    
                    
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
        
      Serial.println(Team_ID + ", " + MISSION_time + ", " + packet_count + ". " + MODE + ", " + state + ", " + alt + ", " + airSpeed + ", " + HS_DEP + ", " + PC_DEP + ", " + temp + ", " + volt + ", " + GPS_time + ", " + GPS_alt + ", " + GPS_long + ", " + GPS_lat + ", " + GPS_sats + ", " + TiltX + ", " + tilty + ", " + rotZ + ", " + CMD_ECHO);
        }
      
    }
}

void cal_alt() {
 
    if(Command.equals("CMD,C,CAL")){
       char data_to_be_sent = "CMD,C,CAL";
       
       tx.setPayload(data_to_be_sent, sizeof(data_to_be_sent));
       tx= ZBTxRequest(addr64, data_to_be_sent, sizeof(data_to_be_sent));
       xbee.send(tx);   

    }

}




void setup() {
  Serial.begin(9600); 
   
}



void processCommand() {

  if (Command.startsWith("CMD," + teamID + ",ST,")) {
   
    String timePart = command.substring(14);
    
    if (timePart.equals("GPS")) {
      
      getTimeFromGPS();
    } else {
      
      setMissionTime(timePart);
    }
  }
}

void getTimeFromGPS() {
 char set_gps_time = "CMD,C,ST,GPS";
       
       tx.setPayload(set_gps_time, sizeof(set_gps_time));
       tx= ZBTxRequest(addr64, set_gps_time, sizeof(set_gps_time));
       xbee.send(tx);  
  
}

void setMissionTime(String time) {
 char set_utc_time = Command;
       
       tx.setPayload(set_utc_time, sizeof(set_gps_time));
       tx= ZBTxRequest(addr64, set_gps_time, sizeof(set_utc_time));
       xbee.send(tx);
  
}
