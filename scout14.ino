const unsigned int ID_NUMBER = 1;

#include <IRremote.hpp>
int IR_RECEIVE_PIN = 11;


#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

RH_ASK driver(2000, 4, 2, 5);//x, receive, transmit,y

int IRpins[] = {A0,A1,A2,A3};
unsigned int IRreadings[] = {0,0,0,0};

String sendBuffer[100];

struct IRData dataReceived;

bool dataReceivedFlag = false;

unsigned int packetNum = 1000;//2 bytes - positive values 0 to 65,535
unsigned long int time = 0;// 4 bytes - positive values 0 to 4,294,967,295

const uint8_t sendCommand = 0xFF;

int freqReadings = 1;//readings per second

void setup(){
  Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // Start the receiver
  if (!driver.init())
     Serial.println("init failed");

  for(int i=0;i<4;i++){
    pinMode(IRpins[i], INPUT);
  }
}

void loop() {
  if ((millis() - time)> 1000*(1/freqReadings)){
    readIntensities(IRreadings, IRpins);
    addToBuffer(sendBuffer, IRreadings, packetNum, time);
    packetNum+=1;
  }
  
  dataReceived = IRreceiver();
  if(dataReceivedFlag){
    Serial.println(dataReceived.address);
    Serial.println(dataReceived.command);
    if (dataReceived.address == ID_NUMBER){
      if(dataReceived.command == sendCommand){
        sendData();
      }
      else{
        uint16_t packetRepeat = dataReceived.command;
        if(packetRepeat >999 && packetRepeat < 1101){
          sendAgain(packetRepeat);
        }
      }
    }
  }
  
  dataReceivedFlag= false;
}

int readIntensities(int * IRreadings, int pins[4]){
  int readings[] = {0,0,0,0};
  for(int i=0;i<4;i++){
    IRreadings[i] = analogRead(pins[i]);
  }
  time = millis();
}

int addToBuffer(String * sendBuffer, unsigned int readings[4], unsigned int packetNum, unsigned long int readingTime){
  char packet[32];
  sprintf(packet, "n%ui%uw%ux%uy%uz%ut%lu", ID_NUMBER, packetNum, IRreadings[0], IRreadings[1], IRreadings[2], IRreadings[3], readingTime);
  sendBuffer[packetNum-1000] = String(packet);
}

void sendData(){
  char x[32];
  for(int i=0; i< 100; i++){
    sendBuffer[i].toCharArray(x,32);
    driver.send((uint8_t *)x, 32);
    driver.waitPacketSent();
  }
  String msg = "x";
  msg.toCharArray(x, 32);
  for(int i=0; i<3; i++){
    driver.send((uint8_t *)x, 32);
    driver.waitPacketSent();
  }
}

void sendAgain(int packet){
  char x[32];
  sendBuffer[packet-1000].toCharArray(x, 32);
  driver.send((uint8_t *)x, 32);
  driver.waitPacketSent();
}



struct IRData IRreceiver(){
  if (IrReceiver.decode()) {
    IrReceiver.resume(); // Enable receiving of the next value
    dataReceivedFlag = true;
    return IrReceiver.decodedIRData;
  }
}

void printIRreadings(int values[4]){
  for(int i=0;i<4;i++){
    Serial.println(i+1);
    Serial.println(values[i]);
  }
}
