/*
hmi.H       
HMI Controller Library for Arduino V2.0
Copyright (c) 2015 Sergio Daniel Castañeda N.

*/


#ifndef hmi_h
#define hmi_h



//#include "Arduino.h"
//#include "HardwareSerial.h"
//#include <StandardCplusplus.h>
#include <vector>
#include <string>
#include <stdio.h>
#include "stm32f7xx_hal.h"
//#include <SoftwareSerial.h>
//#include <Ethernet.h>
//#include <EthernetClient.h>

using namespace std;

/*
#define BIT0MACK 0X1
#define BIT1MACK 0X2
#define BIT2MACK 0X4
#define BIT3MACK 0X8
#define BIT4MACK 0X10
#define BIT5MACK 0X20
#define BIT6MACK 0X40
#define BIT7MACK 0X80
*/

#define BYTE6MASK0 0x3F
#define BYTE6MASK1 0xFC0
#define BYTE6MASK2 0X3F000
#define BYTE6MASK3 0xFC0000
#define BYTE6MASK4 0x3F000000
#define BYTE6MASK5 0xC0000000


#define BYTEMASK0 0xFF
#define BYTEMASK1 0xFF00
#define BYTEMASK2 0xFF0000
#define BYTEMASK3 0xFF000000

#define LDMASK 0x3F
#define MDMASK 0XFC0
#define HDMASK 0xF000

#define HDMASKLDMASK32 0x3F000
#define LDMASK32 0xFC0000
#define MDMASK32 0x3F000000
#define HDMASK32 0xC0000000

#define gmask8 0xFF;
#define gmask16 0xFFFF;


typedef union{
    float val;
    uint8_t bytes[4];
}floatValue;


class Hmi_{
  public:

    int dataChecked;
    uint8_t j;
    uint8_t sizeIncome;
    uint8_t byteData;
    uint8_t ld;
    uint8_t md;
    uint8_t hd;
    uint8_t ld32;
    uint8_t md32;
    uint8_t hd32;
    uint8_t otherData;
    uint8_t cursor;
    uint8_t waitReadCounter;
    uint16_t tempValue;
    uint32_t tempValue32;
    char bufferSerial[128];
    vector<int> inp;
    vector<int> outp;
    vector<boolean> inPS;
    vector<boolean> outPS;

    vector<char> varIn;
    vector<int*> varInV;
    vector<char> varOut;
    vector<int*> varOutV;

    vector<char> varFloatIn;
    vector<float*> varFloatInV;
    vector<char> varFloatOut;
    vector<float*> varFloatOutV;

    vector<char> varLongIn;
    vector<long*> varLongInV;
    vector<char> varLongOut;
    vector<long*> varLongOutV;

    vector<boolean*> logicInV;
    vector<char> logicIn;
    vector<boolean*> logicOutV;
    vector<char> logicOut;
    vector<uint8_t> bitMasks;

    vector<uint8_t> inTimer;
    vector<uint8_t> inBit;
    vector<uint8_t> inPort;
    vector<uint8_t> outTimer;
    vector<uint8_t> outBit;
    vector<uint8_t> outPort;

    String streamCode;
    String letra;
    boolean configStream;
    boolean configStreamEt;
    boolean txStream;
    boolean rxStream;
    boolean inCount;
    boolean waitingAndroid;     //Maybe to delete.



  public:
    Hmi_() {configStream = false; waitingAndroid = false;configStreamEt=false; streamCode="XX"; letra = "A"; txStream=false; waitReadCounter = 0; inCount = true;
        bitMasks.push_back(0X1);
        bitMasks.push_back(0X2);
        bitMasks.push_back(0X4);
        bitMasks.push_back(0X8);
        bitMasks.push_back(0X10);
        bitMasks.push_back(0X20);
        bitMasks.push_back(0X40);
        bitMasks.push_back(0X80);
        };

    void pinMode(uint8_t pin, uint8_t mode);
    void addInPin (int pinNum);
    void addOutPin (int pinNum);
    void getInPins();
    void getOutPins(); 
    void setOutPin();
    void attachIntIn(int &vari, char prenom);
    void attachIntOut(int &vari, char prenom);

    void attachFloatIn(float &vari, char prenom);
    void attachFloatOut(float &vari, char prenom);

    void attachBooleanIn(boolean &logi, char prenom);
    void attachBooleanOut(boolean &logi, char prenom);

    void attachLongIn(long &vari, char prenom);
    void attachLongOut(long &vari, char prenom);
   
    boolean getBoolean(int pos); //Only for debug mode
    void printOutPins();
    void softSerial(SoftwareSerial &serTemp);
    void hardSerial(HardwareSerial &serTemp);
    void nEthernet(EthernetServer servidor);
    void saveWrite(int pinW, int valo);
    void saveRead(int pinW, boolean valo);

    void fragmentData(uint32_t tempInt32, uint8_t basura);
    void fragmentData(uint32_t tempInt16);
    void fragmentData(uint32_t tempInt16, boolean bol, boolean bol2);
    uint8_t joinInt8(char tempMd, char tempLd);
    uint16_t joinInt16(char tempHd, char tempMd, char tempLd);
    uint32_t joinInt32(char temp6, char temp5, char temp4, char temp3, char temp2, char temp1);
    float joinFloat(uint32_t tempInt32);
    int readMyByteUntil(int delimi, SoftwareSerial &serTemp1);
    int readMyByteUntil(int delimi, HardwareSerial &serTemp1);
    int readMyByteUntil(int delimi, EthernetClient &serTemp1);
    int readMyByteUntil(int delimi, EthernetServer &serTemp1);
    void cleanClientData(EthernetServer &serTemp1);
    void cleanSoftSerial(SoftwareSerial &serTemp2);
    void cleanHardSerial(HardwareSerial &serTemp2);


    boolean isConnected();
    boolean readThePin(int pinPos);
    boolean readOutPin(int pinPos);
    void writeOutPin(int pinPos, boolean valOut);
    //void turnMeOffPwm(uint8_t timeR);


};

extern Hmi_ Hmi;

#endif
