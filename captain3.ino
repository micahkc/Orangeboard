#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile
#include <IRremote.hpp>
RH_ASK driver(2000, 2, 4, 5);//x, receive, transmit, y
unsigned int nodeNum = 0;//2 bytes - positive values 0 to 65,535
unsigned int packetID = 0;
uint8_t nodeNum_byte[2];
uint8_t packetID_byte[2];
unsigned int lastPackets[] = {1,1,1,1};

String dataBuffer[100];
int packetTracker[100];



uint8_t sendData = 0xFF;
uint8_t sRepeats = 0;
uint16_t addresses[4] = {0x01,0x02,0x03,0x04};


uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
uint8_t buflen = sizeof(buf);


void setup(){
    Serial.begin(9600);	// Debugging only
    IrSender.begin(3, ENABLE_LED_FEEDBACK);
    if (!driver.init())
         Serial.println("init failed");

    for(int i=0; i<100; i++){
      packetTracker[i] = -1;
    }
}

void loop(){
  for(int i=0; i<4; i++){
    uint16_t addr = addresses[i];
    getData(dataBuffer, packetTracker, addr);
    //requestPackets(dataBuffer, packetTracker, addr);
    //emptyBuffer(dataBuffer);
  }
}

String getData(String * dataBuffer, int * packetTracker, uint16_t address){
  int packet;
  String firstChar;
  bool receiving = true;
  while(receiving){
    Serial.println("sending...");

    IrSender.sendNEC(address & 0xFF, sendData, sRepeats);
    delay(500);
    String rcv = "";
    if (driver.recv(buf, &buflen)){
      driver.printBuffer("Got:", buf, buflen);

      for(int q=0; q<buflen; q++){
        rcv += (char)buf[q];
      }
      firstChar = rcv.substring(0);
        
      Serial.println(firstChar);
      if(firstChar != 'x'){//termination character
        Serial.println("not x");
        packet = (rcv.substring(2,4)).toInt();
        dataBuffer[packet-1000] = rcv;
        packetTracker[packet-1000] = packet;
      }
      else{
        receiving = false;
        Serial.println("x");
      }
    }
    Serial.println(received);
  }
}

String requestPackets(String * dataBuffer, int * packetTracker, uint16_t address){
  int packet;
  for(int i=0; i<100; i++){
    if (packetTracker[i] == -1){
      bool waiting = true;
      while(waiting){
        IrSender.sendNEC(address & 0xFF, i+1000, sRepeats);
        if (driver.recv(buf, &buflen)){
          driver.printBuffer("Got:", buf, buflen);
          String rcv = "";
          for(int i=0; i<buflen; i++){
            rcv += (char)buf[i];
          }
          packet = (rcv.substring(2,4)).toInt();
          dataBuffer[packet-1000] = rcv;
          packetTracker[packet-1000] = packet;
          if (packet = i+1000){
            waiting = false;
          }
        }
      }
    } 
  }
}

void emptyBuffer(String dataBuffer[100]){
  for(int x=0; x<100; x++){
    Serial.println(dataBuffer[x]);
    dataBuffer[x] = "-";
    packetTracker[x] = -1;
  }
}
