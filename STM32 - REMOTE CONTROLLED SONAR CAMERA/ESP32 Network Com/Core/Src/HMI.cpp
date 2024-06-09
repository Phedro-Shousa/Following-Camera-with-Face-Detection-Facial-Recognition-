/*
HMI.cpp
HMI Controller Library for Arduino V2.0
Copyright (c) 2016 Sergio Daniel Castañeda N.

*/

//#include "Arduino.h"


#include "hmi.h"


using namespace std;


void Hmi_::pinMode(uint8_t pin, uint8_t mode){


  uint8_t bit = digitalPinToBitMask(pin);
  uint8_t port = digitalPinToPort(pin);
  volatile uint8_t *reg, *out;

  if (port == NOT_A_PIN) return;

  // JWS: can I let the optimizer do this?
  reg = portModeRegister(port);
  out = portOutputRegister(port);

  if (mode == INPUT) {

    addInPin(pin);

    uint8_t oldSREG = SREG;
                cli();
    *reg &= ~bit;
    *out &= ~bit;
    SREG = oldSREG;
  }

  else if (mode == INPUT_PULLUP) {
    uint8_t oldSREG = SREG;
                cli();
    *reg &= ~bit;
    *out |= bit;
    SREG = oldSREG;
  }

  else {

    addOutPin(pin);

    uint8_t oldSREG = SREG;
                cli();
    *reg |= bit;
    SREG = oldSREG;
  }

}

void Hmi_::addInPin(int pinNum){
    inp.push_back(pinNum);
    inPS.push_back(false);
    inTimer.push_back(digitalPinToTimer(pinNum));
    inBit.push_back(digitalPinToBitMask(pinNum));
    inPort.push_back(digitalPinToPort(pinNum));
}


void Hmi_::addOutPin(int pinNum){
    outp.push_back(pinNum);
    outPS.push_back(false);
    outTimer.push_back(digitalPinToTimer(pinNum));
    outBit.push_back(digitalPinToBitMask(pinNum));
    outPort.push_back(digitalPinToPort(pinNum));
}



void Hmi_::writeOutPin(int pinPos, boolean valOut){

  volatile uint8_t *outD;

  if (outPort.at(pinPos) == NOT_A_PIN) return;

  outD = portOutputRegister(outPort.at(pinPos));

  //uint8_t oldSREG = SREG;
  //cli();

  if (valOut == false) {
    *outD &= ~(outBit.at(pinPos));
  } else {
    *outD |= (outBit.at(pinPos));
  }

  //SREG = oldSREG;
}

boolean Hmi_::isConnected(){
    return configStream;
}

boolean Hmi_::readThePin(int pinPos){


    if (inPort.at(pinPos) == NOT_A_PIN) return false;



    if (*portInputRegister(inPort.at(pinPos)) & (inBit.at(pinPos))) {
      return true;
    }

    else{
      return false;
    }
}

boolean Hmi_::readOutPin(int pinPos){
  volatile uint8_t *outD;

  if (outPort.at(pinPos) == NOT_A_PIN) return false;


  outD = portOutputRegister(outPort.at(pinPos));

  uint8_t resultAdo = (*outD & outBit.at(pinPos));

  if(resultAdo != 0) return true;
  else return false;




}


void Hmi_::getInPins(){

  if(inp.size()>0){
    inPS.clear();
    for(int reco = 0; reco<inp.size();reco++){
      inPS.push_back(readThePin(reco));
    }
  }
}

void Hmi_::getOutPins(){
  if(outp.size()>0){
    outPS.clear();
    for(int reco = 0; reco<outp.size();reco++){
      outPS.push_back(readOutPin(reco));
    }
  }
}


void Hmi_::setOutPin(){
  if(outp.size()>0){
    for(int reco = 0; reco <outp.size();reco++){
      digitalWrite(outp.at(reco),outPS.at(reco));
    }
  }
}


void Hmi_::attachIntIn(int &vari, char prenom){
  varInV.push_back(&vari);
  varIn.push_back(prenom);
}

void Hmi_::attachIntOut(int &vari, char prenom){
  varOutV.push_back(&vari);
  varOut.push_back(prenom);
}

void Hmi_::attachFloatIn(float &vari, char prenom){
  varFloatInV.push_back(&vari);
  varFloatIn.push_back(prenom);
}

void Hmi_::attachFloatOut(float &vari, char prenom){
  varFloatOutV.push_back(&vari);
  varFloatOut.push_back(prenom);
}

void Hmi_::attachLongIn(long &vari, char prenom){
  varLongInV.push_back(&vari);
  varLongIn.push_back(prenom);
}

void Hmi_::attachLongOut(long &vari, char prenom){
  varLongOutV.push_back(&vari);
  varLongOut.push_back(prenom);
}

void Hmi_::attachBooleanIn(boolean &logi, char prenom){   //with this works
  logicInV.push_back(&logi);
  logicIn.push_back(prenom);
}

void Hmi_::attachBooleanOut(boolean &logi, char prenom){
  logicOutV.push_back(&logi);
  logicOut.push_back(prenom);
}


boolean Hmi_::getBoolean(int pos){
  boolean *puntero;
  puntero=Hmi.logicInV.at(pos);
  return *puntero;
}

void Hmi_::printOutPins(){
  if(outp.size()>0){
    for(int i=0; i<outp.size();i++){

      if(outPS.at(i)==true){
        digitalWrite(outp.at(i),HIGH);
      }

      else{
        digitalWrite(outp.at(i),LOW);
      }
    }
  }
} //end printOutPins


void Hmi_::saveWrite(int pinW, int valo){

  for(int k=0; k<outp.size();k++){
    if(outp.at(k)==pinW){
      if(valo == 1){
        outPS.at(k)=true;
      }
      else{
        outPS.at(k)=false;
      }
    }
  }
} //end saveWrite

void Hmi_::saveRead(int pinW, boolean valo){

  for(int k=0; k<inp.size();k++){
    if(inp.at(k)==pinW){
      inPS.at(k)=valo;
    }
  }
} //end saveRead

void Hmi_::fragmentData(uint32_t tempInt32, uint8_t basura){
  ld = (tempInt32 & (BYTE6MASK0));

  tempValue32 = (tempInt32 & (BYTE6MASK1));
  md = (tempValue32 >> 6);

  tempValue32 = (tempInt32 & (BYTE6MASK2));
  hd = (tempValue32 >> 12);

  tempValue32 = (tempInt32 & (BYTE6MASK3));
  ld32 = (tempValue32 >>18);

  tempValue32 = (tempInt32 & (BYTE6MASK4));
  md32 = (tempValue32 >> 24);

  tempValue32 = (tempInt32 & (BYTE6MASK5));
  hd32 = (tempValue32 >> 30);

}

/*
void Hmi_::fragmentData(uint32_t tempInt32, uint8_t basura){
  ld = (tempInt32 & (BYTE7MASK0));

  tempValue32 = (tempInt32 & (BYTE7MASK1));
  md = (tempValue32 >> 7);

  tempValue32 = (tempInt32 & (BYTE7MASK2));
  hd = (tempValue32 >> 14);

  tempValue32 = (tempInt32 & (BYTE7MASK3));
  ld32 = (tempValue32 >>21);

  tempValue32 = (tempInt32 & (BYTE7MASK4));
  md32 = (tempValue32 >> 28);

}
*/

void Hmi_::fragmentData(uint32_t tempInt16){
  ld = (tempInt16 & (LDMASK));

  tempValue = (tempInt16 & (MDMASK));
  md = (tempValue >> 6);

  tempValue = (tempInt16 & (HDMASK));
  hd = (tempValue >> 12);
}


void Hmi_::fragmentData(uint32_t tempInt16, boolean bol, boolean bol2){
  ld = (tempInt16 & (LDMASK));

  tempValue = (tempInt16 & (MDMASK));
  md = (tempValue >> 6);
}

uint8_t Hmi_::joinInt8(char tempMd, char tempLd){
  uint8_t temp = 0;

  uint8_t tempLd1 = (int)tempLd;
  uint8_t tempMd1 = (int)tempMd;

  tempMd1 = (tempMd1 << 6);

  temp = (tempLd1 | tempMd1);
  temp = temp & gmask8;

  return temp;
}

uint32_t Hmi_::joinInt32(char temp6, char temp5, char temp4, char temp3, char temp2, char temp1){
  uint32_t tempA = 0;
  uint32_t tempB = 0;

  tempB = (((int32_t)temp6) << 30);
  tempA = (((int32_t)temp5) << 24);
  tempB = tempB | tempA;
  tempA = (((int32_t)temp4) << 18);
  tempB = tempB | tempA;
  tempA = (((int32_t)temp3) << 12);
  tempB = tempB | tempA;
  tempA = (((int32_t)temp2) << 6);
  tempB = tempB | tempA | ((int32_t)temp1);

  return tempB;
}

float Hmi_::joinFloat(uint32_t tempInt32){
  float temp = 0;
  floatValue tempFloat;

  tempFloat.bytes[0] = (uint8_t)(tempInt32 & BYTEMASK0);
  tempFloat.bytes[1] = (uint8_t)((tempInt32 & BYTEMASK1) >> 8);
  tempFloat.bytes[2] = (uint8_t)((tempInt32 & BYTEMASK2) >> 16);
  tempFloat.bytes[3] = (uint8_t)((tempInt32 & BYTEMASK3) >> 24);

  temp = tempFloat.val;

  return temp;
}


uint16_t Hmi_::joinInt16(char tempHd, char tempMd, char tempLd){
  uint16_t temp = 0;

  uint16_t tempLd1 = (int)tempLd;
  uint16_t tempMd1 = (int)tempMd;
  uint16_t tempHd1 = (int)tempHd;

  tempMd1 = (tempMd1 << 6);
  tempHd1 = (tempHd1 << 12);

  temp = (tempLd1 | tempMd1 | tempHd1);
  temp = temp & gmask16;

  return temp;
}


int Hmi_::readMyByteUntil(int delimi, SoftwareSerial &serTemp1){

    inCount = true;
    int tempBuffSize = sizeof(bufferSerial);
    int countMeBuff = 0;
    int myCountBuff = 0;
    waitReadCounter = 0;

    while(inCount){
      int inSize = serTemp1.available();
      if(inSize > 0){

        for(int i = 0; i<inSize; i++){

          int tempBuffData = serTemp1.read();

          if(tempBuffData != delimi){
            bufferSerial[i + waitReadCounter] = tempBuffData;
            countMeBuff++;
          }

          else{
           inCount = false;
          }

        }
        waitReadCounter += countMeBuff;
      }


      if(myCountBuff > 19){
        inCount = false;
      }

      myCountBuff++;
      delay(50);
    }//END WHILE



    return waitReadCounter;
}

int Hmi_::readMyByteUntil(int delimi, HardwareSerial &serTemp1){

    inCount = true;
    int tempBuffSize = sizeof(bufferSerial);
    int countMeBuff = 0;
    int myCountBuff = 0;
    waitReadCounter = 0;

    while(inCount){
      int inSize = serTemp1.available();
      if(inSize > 0){

        for(int i = 0; i<inSize; i++){

          int tempBuffData = serTemp1.read();

          if(tempBuffData != delimi){
            bufferSerial[i + waitReadCounter] = tempBuffData;
            countMeBuff++;
          }

          else{
           inCount = false;
          }

        }
        waitReadCounter += countMeBuff;
      }


      if(myCountBuff > 19){
        inCount = false;
      }

      myCountBuff++;
      delay(50);
    }//END WHILE



    return waitReadCounter;
}


int Hmi_::readMyByteUntil(int delimi, EthernetClient &serTemp1){

    inCount = true;
    int tempBuffSize = sizeof(bufferSerial);
    int countMeBuff = 0;
    int myCountBuff = 0;
    waitReadCounter = 0;

    while(inCount){
      int inSize = serTemp1.available();
      if(inSize > 0){

        for(int i = 0; i<inSize; i++){

          int tempBuffData = serTemp1.read();

          if(tempBuffData != delimi){
            bufferSerial[i + waitReadCounter] = tempBuffData;
            countMeBuff++;
          }

          else{
           inCount = false;
          }

        }
        waitReadCounter += countMeBuff;
      }


      if(myCountBuff > 19){
        inCount = false;
      }

      myCountBuff++;
      delay(50);
    }//END WHILE



    return waitReadCounter;
}

int Hmi_::readMyByteUntil(int delimi, EthernetServer &serTemp1){

    inCount = true;
    int tempBuffSize = sizeof(bufferSerial);
    int countMeBuff = 0;
    int myCountBuff = 0;
    waitReadCounter = 0;

    while(inCount){

      EthernetClient serTemp2 = serTemp1.available();

      int inSize = serTemp2.available();
      if(inSize > 0){

        for(int i = 0; i<inSize; i++){

          int tempBuffData = serTemp2.read();

          if(tempBuffData != delimi){
            bufferSerial[i + waitReadCounter] = tempBuffData;
            countMeBuff++;
          }

          else{
           inCount = false;
          }

        }
        waitReadCounter += countMeBuff;
      }


      if(myCountBuff > 19){
        inCount = false;
      }

      myCountBuff++;
      delay(50);
    }//END WHILE



    return waitReadCounter;
}

void Hmi_::cleanClientData(EthernetServer &serTemp1){
  EthernetClient serTemp2 = serTemp1.available();

  if(serTemp2.available()){
    while(serTemp2.read() != -1){

    }
  }
}

void Hmi_::cleanSoftSerial(SoftwareSerial &serTemp2){
  if(serTemp2.available()){
    while(serTemp2.read() != -1){

    }
  }
}

void Hmi_::cleanHardSerial(HardwareSerial &serTemp2){
  if(serTemp2.available()){
    while(serTemp2.read() != -1){

    }
  }
}


void Hmi_::softSerial(SoftwareSerial &serTemp){

int buffSize = 0;
 int sizeData = 0;

 if(!configStream){

  if(serTemp.available()>0){

    buffSize = readMyByteUntil(98,serTemp);


    if(bufferSerial[0]=='c'){


      serTemp.write(100);
      serTemp.write('s');
            fragmentData(inp.size(),false,false);
            serTemp.write(md);
            serTemp.write(ld);

            serTemp.write('i');
             if(inp.size()>0){                                     //Pin Entrada
              for(int forint = 0; forint<inp.size(); forint++){
                  fragmentData(inp.at(forint),false,false);
                  serTemp.write(md);
                  serTemp.write(ld);
              }
           }


          fragmentData(outp.size(),false);
            serTemp.write(md);
            serTemp.write(ld);
          serTemp.write('o');
           if(outp.size()>0){                                    //Pin Salida
              for(int forint = 0; forint<outp.size(); forint++){
                fragmentData(outp.at(forint),false,false);
                  serTemp.write(md);
                  serTemp.write(ld);
              }
           }

            fragmentData(varIn.size(),false,false);
            serTemp.write(md);
            serTemp.write(ld);
            serTemp.write('a');

            if(varIn.size()>0){                                   //Variable Entrada
              for(int forint = 0; forint<varIn.size(); forint++){
                fragmentData(varIn.at(forint),false,false);
                  serTemp.write(md);
                  serTemp.write(ld);
              }
           }

            fragmentData(varOut.size(),false,false);
            serTemp.write(md);
            serTemp.write(ld);
            serTemp.write('v');

            if(varOut.size()>0){                                   //Variable Salida
              for(int forint = 0; forint<varOut.size(); forint++){
                fragmentData(varOut.at(forint),false,false);
                  serTemp.write(md);
                  serTemp.write(ld);
              }
           }

            fragmentData(logicIn.size(),false,false);
            serTemp.write(md);
            serTemp.write(ld);
            serTemp.write('e');

            if(logicIn.size()>0){                                  //Logico Entrada
              for(int forint = 0; forint<logicIn.size(); forint++){
                fragmentData(logicIn.at(forint),false,false);
                  serTemp.write(md);
                  serTemp.write(ld);
              }
           }

            fragmentData(logicOut.size(),false,false);
            serTemp.write(md);
            serTemp.write(ld);
            serTemp.write('l');

            if(logicOut.size()>0){                                   //Logico Salida
              for(int forint = 0; forint<logicOut.size(); forint++){
                fragmentData(logicOut.at(forint),false,false);
                  serTemp.write(md);
                  serTemp.write(ld);
              }
           }



           fragmentData(varFloatIn.size(), false, false);
           serTemp.write(md);
           serTemp.write(ld);
           serTemp.write('n');

           if(varFloatIn.size()>0){
            for(int forint = 0; forint<varFloatIn.size(); forint++){
              fragmentData(varFloatIn.at(forint), false, false);
                serTemp.write(md);
                serTemp.write(ld);
            }
           }

           fragmentData(varFloatOut.size(), false,false);
           serTemp.write(md);
           serTemp.write(ld);
           serTemp.write('p');

           if(varFloatOut.size() > 0){
            for(int forint = 0; forint<varFloatOut.size(); forint++){
              fragmentData(varFloatOut.at(forint), false, false);
              serTemp.write(md);
              serTemp.write(ld);
            }
           }


          fragmentData(varLongIn.size(), false, false);
           serTemp.write(md);
           serTemp.write(ld);
           serTemp.write('q');

           if(varLongIn.size()>0){
            for(int forint = 0; forint<varLongIn.size(); forint++){
              fragmentData(varLongIn.at(forint),false, false);
                serTemp.write(md);
                serTemp.write(ld);
            }
           }

           fragmentData(varLongOut.size(), false,false);
           serTemp.write(md);
           serTemp.write(ld);
           serTemp.write('r');

           if(varLongOut.size() > 0){
            for(int forint = 0; forint<varLongOut.size(); forint++){
              fragmentData(varLongOut.at(forint), false, false);
              serTemp.write(md);
              serTemp.write(ld);
            }
           }


           serTemp.write(98);

          serTemp.flush();

        
    } //END if(bufferSerial[0]=='c')

    else if(bufferSerial[0]=='f'){
      serTemp.flush();
      configStream = true;
    }

  }//END if(serTemp.available()>0)
  }//END if(!configStream)


  else{

    rxStream = true;
    dataChecked = 0;
    int tempMultipli = 0;

    cleanSoftSerial(serTemp);

    //ARDUINO to ANDROID

      serTemp.write(100); //KBCRA
      serTemp.write('g');

            fragmentData(inPS.size(),false,false);
            serTemp.write(md);
            serTemp.write(ld);



            serTemp.write('i');

             if(inPS.size()>0){                                     //Pin Entrada

              byteData = 0; //reset byteData
              j= 0;
              int multi = 1;
              tempMultipli = ((6*multi)-1);
              getInPins();

              for(int forint = 0; forint<inPS.size(); forint++){

                boolean dataTemp = inPS.at(forint);

                if(dataTemp){
                  byteData = bitMasks.at(j) | byteData;
                }

                j++;

                if(forint == tempMultipli){
                  j = 0;
                  serTemp.write(byteData);
 
                  byteData = 0;
                  multi++;
                  tempMultipli = ((6*multi)-1);
                }

              }//END for(forint = 0)

              if(j<6){  //There still been a byte to send
                serTemp.write(byteData);

              }

           }//END if(inPS.size)




           fragmentData(outPS.size(),false,false);
            serTemp.write(md);
            serTemp.write(ld);

            serTemp.write('o');

            if(outPS.size()>0){                                     //Pin Salida

              byteData = 0; //reset byteData
              j= 0;
              int multi = 1;
              tempMultipli = ((6*multi)-1);
              getOutPins();

              for(int forint = 0; forint<outPS.size(); forint++){

                boolean dataTemp = outPS.at(forint);

                if(dataTemp){
                  byteData = bitMasks.at(j) | byteData;
                }

                j++;

                if(forint == tempMultipli){
                  j = 0;
                  serTemp.write(byteData);
                  byteData = 0;
                  multi++;
                  tempMultipli = ((6*multi)-1);
                }

              }//END for(forint = 0)

              if(j<6){  //There still been a byte to send
                serTemp.write(byteData);
              }

            }//END if(outPS.size)




            fragmentData(varOutV.size(),false,false);
            serTemp.write(md);
            serTemp.write(ld);

            serTemp.write('v');

            if(varOutV.size()>0){                                     //Variable Salida


              for(int forint = 0; forint<varOutV.size(); forint++){
                int dataTemp = *varOutV.at(forint);
                fragmentData(dataTemp);
                serTemp.write(hd);
                serTemp.write(md);
                serTemp.write(ld);

              }//END for(forint = 0)
            }//END if(varOutV.size)



            fragmentData(logicOutV.size(),false,false);
            serTemp.write(md);
            serTemp.write(ld);

            serTemp.write('l');

            if(logicOutV.size()>0){                                     //Logico Salida

              byteData = 0; //reset byteData
              j= 0;
              int multi = 1;
              tempMultipli = ((6*multi)-1);


              for(int forint = 0; forint<logicOutV.size(); forint++){

                boolean dataTemp = *logicOutV.at(forint);

                if(dataTemp){
                  byteData = bitMasks.at(j) | byteData;
                }

                j++;

                if(forint == tempMultipli){
                  j = 0;
                  serTemp.write(byteData);
                  byteData = 0;
                  multi++;
                  tempMultipli = ((6*multi)-1);
                }

              }//END for(forint = 0)

              if(j<6){  //There still been a byte to send
                serTemp.write(byteData);
              }

            }//END if(logicOutV.size)


            fragmentData(varLongOutV.size(), false, false);               //Long Salida
            serTemp.write(md);
            serTemp.write(ld);

            serTemp.write('r');

            if(varLongOutV.size()>0){
              for(int forint = 0; forint < varLongOutV.size(); forint++){
                uint32_t dataTemp = *varLongOutV.at(forint);
                fragmentData(dataTemp, (uint8_t)4);
                serTemp.write(hd32);
                serTemp.write(md32);
                serTemp.write(ld32);
                serTemp.write(hd);
                serTemp.write(md);
                serTemp.write(ld);
              }

            }

            fragmentData(varFloatOutV.size(), false, false);
            serTemp.write(md);
            serTemp.write(ld);

            serTemp.write('p');

            if(varFloatOutV.size()>0){
              for(int forint = 0; forint < varFloatOutV.size(); forint++){
                floatValue tempFloat;
                tempFloat.val = *varFloatOutV.at(forint);

                uint32_t tempUint32 = (uint32_t)tempFloat.bytes[0];
                tempUint32 |= ((uint32_t)tempFloat.bytes[1] << 8);
                tempUint32 |= ((uint32_t)tempFloat.bytes[2] << 16);
                tempUint32 |= ((uint32_t)tempFloat.bytes[3] << 24);

                fragmentData(tempUint32, (uint8_t)4);
                serTemp.write(hd32);
                serTemp.write(md32);
                serTemp.write(ld32);
                serTemp.write(hd);
                serTemp.write(md);
                serTemp.write(ld);


              }
            }


      serTemp.write(98);  //DELIMITADOR


      dataChecked = buffSize = readMyByteUntil(98,serTemp);
  
      if(dataChecked>0){
        if(bufferSerial[0] == 'f'){
          txStream = false;
          cleanSoftSerial(serTemp);
        }//END if(bufferSerial)
      }//END if(sincroChecked)


    if(rxStream){

    serTemp.write(100); //KBCRA
    serTemp.write('h');
    serTemp.write('h');
    serTemp.write(98);


    //unsigned long  start = millis();
    while(rxStream){
  


      if(serTemp.available()){
        //unsigned long end = millis();
        //unsigned long delta = end - start;
        //start = millis();

        dataChecked = buffSize = readMyByteUntil(98,serTemp);

        if(dataChecked>0){


          switch(bufferSerial[0]){

            case 'o':
  
              sizeIncome=joinInt8(bufferSerial[1], bufferSerial[2]);
              cursor = 3;

              if(sizeIncome > 0){
                if(sizeIncome ==  outPS.size()){

                  uint8_t tempMd2 = bufferSerial[cursor];
                  cursor++;
                  uint8_t tempLd2 = bufferSerial[cursor];
                  cursor++;

                  otherData = joinInt8(tempMd2, tempLd2);

                  outPS.clear();
                  j = 0;
                  int multi = 1;
                  tempMultipli = ((8*multi)-1);


                  for(int forint = 0; forint < sizeIncome; forint++){
                    int dataTemp = (bitMasks.at(j) & otherData);


                    if(dataTemp !=0) {
                      outPS.push_back(true);
                      writeOutPin(forint,true);
                    }
                    else {
                      outPS.push_back(false);
                      writeOutPin(forint,false);
                    }

                    j++;

                    if((forint == tempMultipli) && (forint != (sizeIncome-1))){

                      uint8_t tempMd2 = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempLd2 = bufferSerial[cursor];
                      cursor++;

                      otherData = joinInt8(tempMd2, tempLd2);

                      j=0;
                      multi++;
                      tempMultipli = ((8*multi)-1);
                    }//END if()

                  } //END for(int forint = 0; forint < sizeIncome; forint++)
                  cursor++;
                }//END if(sizeIncome == outPS.size())
              } //END if(sizeIncome > 0)

              serTemp.write(100);
              serTemp.write('h');
              serTemp.write('j');
              serTemp.write(98);


              break;

            case 'e':
    
              sizeIncome=joinInt8(bufferSerial[1], bufferSerial[2]);
              cursor = 3;

              if(sizeIncome > 0){
                if(sizeIncome ==  logicInV.size()){

                  uint8_t tempMd2 = bufferSerial[cursor];
                  cursor++;
                  uint8_t tempLd2 = bufferSerial[cursor];
                  cursor++;

                  otherData = joinInt8(tempMd2, tempLd2);

                  j = 0;
                  int multi = 1;
                  tempMultipli = ((8*multi)-1);

                  for(int forint = 0; forint < sizeIncome; forint++){
                    int dataTemp = (bitMasks.at(j) & otherData);

                    if(dataTemp !=0) *logicInV.at(forint)= true;
                    else *logicInV.at(forint)=false;

                    j++;

                    if((forint == tempMultipli) && (forint != (sizeIncome-1))){

                      uint8_t tempMd2 = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempLd2 = bufferSerial[cursor];
                      cursor++;

                      otherData = joinInt8(tempMd2, tempLd2);

                      j=0;
                      multi++;
                      tempMultipli = ((8*multi)-1);
                    }//END if()

                  } //END for(int forint = 0; forint < sizeIncome; forint++)
                  cursor++;
                }//END if(sizeIncome == outPS.size())
              } //END if(sizeIncome > 0)

              serTemp.write(100);
              serTemp.write('h');
              serTemp.write('k');
              serTemp.write(98);

              break;

            case 'a':
              sizeIncome=joinInt8(bufferSerial[1], bufferSerial[2]);
              cursor = 3;

              if(sizeIncome > 0){
                if(sizeIncome ==  varInV.size()){
                  for(int forint =0; forint < sizeIncome; forint++){
                    uint8_t tempHd2 = bufferSerial[cursor];
                    cursor++;
                    uint8_t tempMd2 = bufferSerial[cursor];
                    cursor++;
                    uint8_t tempLd2 = bufferSerial[cursor];
                    cursor++;

                    *varInV.at(forint) = joinInt16(tempHd2, tempMd2, tempLd2);

                  }//END for(int forint =0; forint < sizeIncome; forint++)
                }//END if(sizeIncome ==  varInV.size())
              }//END if(sizeIncome > 0)

              serTemp.write(100);
              serTemp.write('h');
              serTemp.write('t');
              serTemp.write(98);

              break;

              case 'q':                                             //long input

                sizeIncome=joinInt8(bufferSerial[1], bufferSerial[2]);
                cursor = 3;

                if(sizeIncome > 0){
                  if(sizeIncome ==  varLongInV.size()){
                    for(int forint =0; forint < sizeIncome; forint++){
                      uint8_t tempHd2 = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempMd2 = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempLd2 = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempHd = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempMd = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempLd = bufferSerial[cursor];
                      cursor++;

                    
                      long tempLong = joinInt32(tempHd2, tempMd2, tempLd2, tempHd, tempMd, tempLd);
                  
                      *varLongInV.at(forint) = tempLong;

                    }//END for(int forint =0; forint < sizeIncome; forint++)
                  }//END if(sizeIncome ==  varInV.size())
                }//END if(sizeIncome > 0)

                serTemp.write(100);
                serTemp.write('h');
                serTemp.write('x');
                serTemp.write(98);  
              break;


              case 'n':
                sizeIncome=joinInt8(bufferSerial[1], bufferSerial[2]);
                cursor = 3;

                if(sizeIncome > 0){
                  if(sizeIncome ==  varFloatInV.size()){
                    for(int forint =0; forint < sizeIncome; forint++){
                      uint8_t tempHd2 = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempMd2 = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempLd2 = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempHd = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempMd = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempLd = bufferSerial[cursor];
                      cursor++;
 
                      uint32_t tempUint32 = joinInt32(tempHd2, tempMd2, tempLd2, tempHd, tempMd, tempLd);
                      float tempFloat = joinFloat(tempUint32);
                      *varFloatInV.at(forint) = tempFloat;

                    }//END for(int forint =0; forint < sizeIncome; forint++)
                  }//END if(sizeIncome ==  varInV.size())
                }//END if(sizeIncome > 0)


                rxStream = false;

              break;

            case 'm':
              rxStream = false;
              break;

              default:
                rxStream = false;
              break;

          }//END switch(bufferSerial)
        }//END if(sincroChecked)

      }//END if(serTemp.available())

    }//END while(rxStream)


  }//END if(rxStream)

  }//END else


}//end sincroSoft


void Hmi_::hardSerial(HardwareSerial &serTemp){

 int buffSize = 0;
 int sizeData = 0;

 if(!configStream){

  if(serTemp.available()>0){

    buffSize = readMyByteUntil(98,serTemp);


    if(bufferSerial[0]=='c'){


      serTemp.write(100);
      serTemp.write('s');
            fragmentData(inp.size(),false,false);
            serTemp.write(md);
            serTemp.write(ld);

            serTemp.write('i');
             if(inp.size()>0){                                     //Pin Entrada
              for(int forint = 0; forint<inp.size(); forint++){
                  fragmentData(inp.at(forint),false,false);
                  serTemp.write(md);
                  serTemp.write(ld);
              }
           }


          fragmentData(outp.size(),false);
            serTemp.write(md);
            serTemp.write(ld);
          serTemp.write('o');
           if(outp.size()>0){                                    //Pin Salida
              for(int forint = 0; forint<outp.size(); forint++){
                fragmentData(outp.at(forint),false,false);
                  serTemp.write(md);
                  serTemp.write(ld);
              }
           }

            fragmentData(varIn.size(),false,false);
            serTemp.write(md);
            serTemp.write(ld);
            serTemp.write('a');

            if(varIn.size()>0){                                   //Variable Entrada
              for(int forint = 0; forint<varIn.size(); forint++){
                fragmentData(varIn.at(forint),false,false);
                  serTemp.write(md);
                  serTemp.write(ld);
              }
           }

            fragmentData(varOut.size(),false,false);
            serTemp.write(md);
            serTemp.write(ld);
            serTemp.write('v');

            if(varOut.size()>0){                                   //Variable Salida
              for(int forint = 0; forint<varOut.size(); forint++){
                fragmentData(varOut.at(forint),false,false);
                  serTemp.write(md);
                  serTemp.write(ld);
              }
           }

            fragmentData(logicIn.size(),false,false);
            serTemp.write(md);
            serTemp.write(ld);
            serTemp.write('e');

            if(logicIn.size()>0){                                  //Logico Entrada
              for(int forint = 0; forint<logicIn.size(); forint++){
                fragmentData(logicIn.at(forint),false,false);
                  serTemp.write(md);
                  serTemp.write(ld);
              }
           }

            fragmentData(logicOut.size(),false,false);
            serTemp.write(md);
            serTemp.write(ld);
            serTemp.write('l');

            if(logicOut.size()>0){                                   //Logico Salida
              for(int forint = 0; forint<logicOut.size(); forint++){
                fragmentData(logicOut.at(forint),false,false);
                  serTemp.write(md);
                  serTemp.write(ld);
              }
           }



           fragmentData(varFloatIn.size(), false, false);
           serTemp.write(md);
           serTemp.write(ld);
           serTemp.write('n');

           if(varFloatIn.size()>0){
            for(int forint = 0; forint<varFloatIn.size(); forint++){
              fragmentData(varFloatIn.at(forint), false, false);
                serTemp.write(md);
                serTemp.write(ld);
            }
           }

           fragmentData(varFloatOut.size(), false,false);
           serTemp.write(md);
           serTemp.write(ld);
           serTemp.write('p');

           if(varFloatOut.size() > 0){
            for(int forint = 0; forint<varFloatOut.size(); forint++){
              fragmentData(varFloatOut.at(forint), false, false);
              serTemp.write(md);
              serTemp.write(ld);
            }
           }


          fragmentData(varLongIn.size(), false, false);
           serTemp.write(md);
           serTemp.write(ld);
           serTemp.write('q');

           if(varLongIn.size()>0){
            for(int forint = 0; forint<varLongIn.size(); forint++){
              fragmentData(varLongIn.at(forint),false, false);
                serTemp.write(md);
                serTemp.write(ld);
            }
           }

           fragmentData(varLongOut.size(), false,false);
           serTemp.write(md);
           serTemp.write(ld);
           serTemp.write('r');

           if(varLongOut.size() > 0){
            for(int forint = 0; forint<varLongOut.size(); forint++){
              fragmentData(varLongOut.at(forint), false, false);
              serTemp.write(md);
              serTemp.write(ld);
            }
           }


           serTemp.write(98);

          serTemp.flush();

        
    } //END if(bufferSerial[0]=='c')

    else if(bufferSerial[0]=='f'){
      serTemp.flush();
      configStream = true;
    }

  }//END if(serTemp.available()>0)
  }//END if(!configStream)


  else{

    rxStream = true;
    dataChecked = 0;
    int tempMultipli = 0;

    cleanHardSerial(serTemp);

    //ARDUINO to ANDROID

      serTemp.write(100); //KBCRA
      serTemp.write('g');

            fragmentData(inPS.size(),false,false);
            serTemp.write(md);
            serTemp.write(ld);



            serTemp.write('i');

             if(inPS.size()>0){                                     //Pin Entrada

              byteData = 0; //reset byteData
              j= 0;
              int multi = 1;
              tempMultipli = ((6*multi)-1);
              getInPins();

              for(int forint = 0; forint<inPS.size(); forint++){

                boolean dataTemp = inPS.at(forint);

                if(dataTemp){
                  byteData = bitMasks.at(j) | byteData;
                }

                j++;

                if(forint == tempMultipli){
                  j = 0;
                  serTemp.write(byteData);
 
                  byteData = 0;
                  multi++;
                  tempMultipli = ((6*multi)-1);
                }

              }//END for(forint = 0)

              if(j<6){  //There still been a byte to send
                serTemp.write(byteData);

              }

           }//END if(inPS.size)




           fragmentData(outPS.size(),false,false);
            serTemp.write(md);
            serTemp.write(ld);

            serTemp.write('o');

            if(outPS.size()>0){                                     //Pin Salida

              byteData = 0; //reset byteData
              j= 0;
              int multi = 1;
              tempMultipli = ((6*multi)-1);
              getOutPins();

              for(int forint = 0; forint<outPS.size(); forint++){

                boolean dataTemp = outPS.at(forint);

                if(dataTemp){
                  byteData = bitMasks.at(j) | byteData;
                }

                j++;

                if(forint == tempMultipli){
                  j = 0;
                  serTemp.write(byteData);
                  byteData = 0;
                  multi++;
                  tempMultipli = ((6*multi)-1);
                }

              }//END for(forint = 0)

              if(j<6){  //There still been a byte to send
                serTemp.write(byteData);
              }

            }//END if(outPS.size)




            fragmentData(varOutV.size(),false,false);
            serTemp.write(md);
            serTemp.write(ld);

            serTemp.write('v');

            if(varOutV.size()>0){                                     //Variable Salida


              for(int forint = 0; forint<varOutV.size(); forint++){
                int dataTemp = *varOutV.at(forint);
                fragmentData(dataTemp);
                serTemp.write(hd);
                serTemp.write(md);
                serTemp.write(ld);

              }//END for(forint = 0)
            }//END if(varOutV.size)



            fragmentData(logicOutV.size(),false,false);
            serTemp.write(md);
            serTemp.write(ld);

            serTemp.write('l');

            if(logicOutV.size()>0){                                     //Logico Salida

              byteData = 0; //reset byteData
              j= 0;
              int multi = 1;
              tempMultipli = ((6*multi)-1);


              for(int forint = 0; forint<logicOutV.size(); forint++){

                boolean dataTemp = *logicOutV.at(forint);

                if(dataTemp){
                  byteData = bitMasks.at(j) | byteData;
                }

                j++;

                if(forint == tempMultipli){
                  j = 0;
                  serTemp.write(byteData);
                  byteData = 0;
                  multi++;
                  tempMultipli = ((6*multi)-1);
                }

              }//END for(forint = 0)

              if(j<6){  //There still been a byte to send
                serTemp.write(byteData);
              }

            }//END if(logicOutV.size)


            fragmentData(varLongOutV.size(), false, false);               //Long Salida
            serTemp.write(md);
            serTemp.write(ld);

            serTemp.write('r');

            if(varLongOutV.size()>0){
              for(int forint = 0; forint < varLongOutV.size(); forint++){
                uint32_t dataTemp = *varLongOutV.at(forint);
                fragmentData(dataTemp, (uint8_t)4);
                serTemp.write(hd32);
                serTemp.write(md32);
                serTemp.write(ld32);
                serTemp.write(hd);
                serTemp.write(md);
                serTemp.write(ld);
              }

            }

            fragmentData(varFloatOutV.size(), false, false);
            serTemp.write(md);
            serTemp.write(ld);

            serTemp.write('p');

            if(varFloatOutV.size()>0){
              for(int forint = 0; forint < varFloatOutV.size(); forint++){
                floatValue tempFloat;
                tempFloat.val = *varFloatOutV.at(forint);

                uint32_t tempUint32 = (uint32_t)tempFloat.bytes[0];
                tempUint32 |= ((uint32_t)tempFloat.bytes[1] << 8);
                tempUint32 |= ((uint32_t)tempFloat.bytes[2] << 16);
                tempUint32 |= ((uint32_t)tempFloat.bytes[3] << 24);

                fragmentData(tempUint32, (uint8_t)4);
                serTemp.write(hd32);
                serTemp.write(md32);
                serTemp.write(ld32);
                serTemp.write(hd);
                serTemp.write(md);
                serTemp.write(ld);


              }
            }


      serTemp.write(98);  //DELIMITADOR


      dataChecked = buffSize = readMyByteUntil(98,serTemp);
  
      if(dataChecked>0){
        if(bufferSerial[0] == 'f'){
          txStream = false;
          cleanHardSerial(serTemp);
        }//END if(bufferSerial)
      }//END if(sincroChecked)


    if(rxStream){

    serTemp.write(100); //KBCRA
    serTemp.write('h');
    serTemp.write('h');
    serTemp.write(98);


    //unsigned long  start = millis();
    while(rxStream){
  


      if(serTemp.available()){
        //unsigned long end = millis();
        //unsigned long delta = end - start;
        //start = millis();

        dataChecked = buffSize = readMyByteUntil(98,serTemp);

        if(dataChecked>0){


          switch(bufferSerial[0]){

            case 'o':
  
              sizeIncome=joinInt8(bufferSerial[1], bufferSerial[2]);
              cursor = 3;

              if(sizeIncome > 0){
                if(sizeIncome ==  outPS.size()){

                  uint8_t tempMd2 = bufferSerial[cursor];
                  cursor++;
                  uint8_t tempLd2 = bufferSerial[cursor];
                  cursor++;

                  otherData = joinInt8(tempMd2, tempLd2);

                  outPS.clear();
                  j = 0;
                  int multi = 1;
                  tempMultipli = ((8*multi)-1);


                  for(int forint = 0; forint < sizeIncome; forint++){
                    int dataTemp = (bitMasks.at(j) & otherData);


                    if(dataTemp !=0) {
                      outPS.push_back(true);
                      writeOutPin(forint,true);
                    }
                    else {
                      outPS.push_back(false);
                      writeOutPin(forint,false);
                    }

                    j++;

                    if((forint == tempMultipli) && (forint != (sizeIncome-1))){

                      uint8_t tempMd2 = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempLd2 = bufferSerial[cursor];
                      cursor++;

                      otherData = joinInt8(tempMd2, tempLd2);

                      j=0;
                      multi++;
                      tempMultipli = ((8*multi)-1);
                    }//END if()

                  } //END for(int forint = 0; forint < sizeIncome; forint++)
                  cursor++;
                }//END if(sizeIncome == outPS.size())
              } //END if(sizeIncome > 0)

              serTemp.write(100);
              serTemp.write('h');
              serTemp.write('j');
              serTemp.write(98);


              break;

            case 'e':
    
              sizeIncome=joinInt8(bufferSerial[1], bufferSerial[2]);
              cursor = 3;

              if(sizeIncome > 0){
                if(sizeIncome ==  logicInV.size()){

                  uint8_t tempMd2 = bufferSerial[cursor];
                  cursor++;
                  uint8_t tempLd2 = bufferSerial[cursor];
                  cursor++;

                  otherData = joinInt8(tempMd2, tempLd2);

                  j = 0;
                  int multi = 1;
                  tempMultipli = ((8*multi)-1);

                  for(int forint = 0; forint < sizeIncome; forint++){
                    int dataTemp = (bitMasks.at(j) & otherData);

                    if(dataTemp !=0) *logicInV.at(forint)= true;
                    else *logicInV.at(forint)=false;

                    j++;

                    if((forint == tempMultipli) && (forint != (sizeIncome-1))){

                      uint8_t tempMd2 = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempLd2 = bufferSerial[cursor];
                      cursor++;

                      otherData = joinInt8(tempMd2, tempLd2);

                      j=0;
                      multi++;
                      tempMultipli = ((8*multi)-1);
                    }//END if()

                  } //END for(int forint = 0; forint < sizeIncome; forint++)
                  cursor++;
                }//END if(sizeIncome == outPS.size())
              } //END if(sizeIncome > 0)

              serTemp.write(100);
              serTemp.write('h');
              serTemp.write('k');
              serTemp.write(98);

              break;

            case 'a':
              sizeIncome=joinInt8(bufferSerial[1], bufferSerial[2]);
              cursor = 3;

              if(sizeIncome > 0){
                if(sizeIncome ==  varInV.size()){
                  for(int forint =0; forint < sizeIncome; forint++){
                    uint8_t tempHd2 = bufferSerial[cursor];
                    cursor++;
                    uint8_t tempMd2 = bufferSerial[cursor];
                    cursor++;
                    uint8_t tempLd2 = bufferSerial[cursor];
                    cursor++;

                    *varInV.at(forint) = joinInt16(tempHd2, tempMd2, tempLd2);

                  }//END for(int forint =0; forint < sizeIncome; forint++)
                }//END if(sizeIncome ==  varInV.size())
              }//END if(sizeIncome > 0)

              serTemp.write(100);
              serTemp.write('h');
              serTemp.write('t');
              serTemp.write(98);

              break;

              case 'q':                                             //long input

                sizeIncome=joinInt8(bufferSerial[1], bufferSerial[2]);
                cursor = 3;

                if(sizeIncome > 0){
                  if(sizeIncome ==  varLongInV.size()){
                    for(int forint =0; forint < sizeIncome; forint++){
                      uint8_t tempHd2 = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempMd2 = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempLd2 = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempHd = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempMd = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempLd = bufferSerial[cursor];
                      cursor++;

                    
                      long tempLong = joinInt32(tempHd2, tempMd2, tempLd2, tempHd, tempMd, tempLd);
                  
                      *varLongInV.at(forint) = tempLong;

                    }//END for(int forint =0; forint < sizeIncome; forint++)
                  }//END if(sizeIncome ==  varInV.size())
                }//END if(sizeIncome > 0)

                serTemp.write(100);
                serTemp.write('h');
                serTemp.write('x');
                serTemp.write(98);  
              break;


              case 'n':
                sizeIncome=joinInt8(bufferSerial[1], bufferSerial[2]);
                cursor = 3;

                if(sizeIncome > 0){
                  if(sizeIncome ==  varFloatInV.size()){
                    for(int forint =0; forint < sizeIncome; forint++){
                      uint8_t tempHd2 = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempMd2 = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempLd2 = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempHd = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempMd = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempLd = bufferSerial[cursor];
                      cursor++;
 
                      uint32_t tempUint32 = joinInt32(tempHd2, tempMd2, tempLd2, tempHd, tempMd, tempLd);
                      float tempFloat = joinFloat(tempUint32);
                      *varFloatInV.at(forint) = tempFloat;

                    }//END for(int forint =0; forint < sizeIncome; forint++)
                  }//END if(sizeIncome ==  varInV.size())
                }//END if(sizeIncome > 0)


                rxStream = false;

              break;

            case 'm':
              rxStream = false;
              break;

              default:
                rxStream = false;
              break;

          }//END switch(bufferSerial)
        }//END if(sincroChecked)

      }//END if(serTemp.available())

    }//END while(rxStream)


  }//END if(rxStream)

  }//END else

} //end sincroHard





void Hmi_::nEthernet(EthernetServer serTemp){

  int buffSize = 0;
  int sizeData = 0;

  if(!configStream){

  EthernetClient clientTemp = serTemp.available();

  if(clientTemp.available()>0){
    buffSize = readMyByteUntil(98, clientTemp);


    if(bufferSerial[0]=='c'){


      clientTemp.write(100);
      clientTemp.write('s');
            fragmentData(inp.size(),false,false);
            clientTemp.write(md);
            clientTemp.write(ld);

            clientTemp.write('i');
             if(inp.size()>0){                                     //Pin Entrada
              for(int forint = 0; forint<inp.size(); forint++){
                  fragmentData(inp.at(forint),false,false);
                  clientTemp.write(md);
                  clientTemp.write(ld);
              }
           }


          fragmentData(outp.size(),false,false);
            clientTemp.write(md);
            clientTemp.write(ld);
          clientTemp.write('o');
           if(outp.size()>0){                                    //Pin Salida
              for(int forint = 0; forint<outp.size(); forint++){
                fragmentData(outp.at(forint),false,false);
                  clientTemp.write(md);
                  clientTemp.write(ld);
              }
           }

            fragmentData(varIn.size(),false,false);
            clientTemp.write(md);
            clientTemp.write(ld);
            clientTemp.write('a');

            if(varIn.size()>0){                                   //Variable Entrada
              for(int forint = 0; forint<varIn.size(); forint++){
                fragmentData(varIn.at(forint),false,false);
                  clientTemp.write(md);
                  clientTemp.write(ld);
              }
           }

            fragmentData(varOut.size(),false,false);
            clientTemp.write(md);
            clientTemp.write(ld);
            clientTemp.write('v');

            if(varOut.size()>0){                                   //Variable Salida
              for(int forint = 0; forint<varOut.size(); forint++){
                fragmentData(varOut.at(forint),false,false);
                  clientTemp.write(md);
                  clientTemp.write(ld);
              }
           }

            fragmentData(logicIn.size(),false,false);
            clientTemp.write(md);
            clientTemp.write(ld);
            clientTemp.write('e');

            if(logicIn.size()>0){                                  //Logico Entrada
              for(int forint = 0; forint<logicIn.size(); forint++){
                fragmentData(logicIn.at(forint),false,false);
                  clientTemp.write(md);
                  clientTemp.write(ld);
              }
           }

            fragmentData(logicOut.size(),false,false);
            clientTemp.write(md);
            clientTemp.write(ld);
            clientTemp.write('l');

            if(logicOut.size()>0){                                   //Logico Salida
              for(int forint = 0; forint<logicOut.size(); forint++){
                fragmentData(logicOut.at(forint),false,false);
                  clientTemp.write(md);
                  clientTemp.write(ld);
              }
           }

           fragmentData(varFloatIn.size(), false, false);
           clientTemp.write(md);
           clientTemp.write(ld);
           clientTemp.write('n');

           if(varFloatIn.size()>0){
            for(int forint = 0; forint<varFloatIn.size(); forint++){
              fragmentData(varFloatIn.at(forint), false, false);
                clientTemp.write(md);
                clientTemp.write(ld);
            }
           }

           fragmentData(varFloatOut.size(), false,false);
           clientTemp.write(md);
           clientTemp.write(ld);
           clientTemp.write('p');

           if(varFloatOut.size() > 0){
            for(int forint = 0; forint<varFloatOut.size(); forint++){
              fragmentData(varFloatOut.at(forint), false, false);
              clientTemp.write(md);
              clientTemp.write(ld);
            }
           }


          fragmentData(varLongIn.size(), false, false);
           clientTemp.write(md);
           clientTemp.write(ld);
           clientTemp.write('q');

           if(varLongIn.size()>0){
            for(int forint = 0; forint<varLongIn.size(); forint++){
              fragmentData(varLongIn.at(forint),false, false);
                clientTemp.write(md);
                clientTemp.write(ld);
            }
           }

           fragmentData(varLongOut.size(), false,false);
           clientTemp.write(md);
           clientTemp.write(ld);
           clientTemp.write('r');

           if(varLongOut.size() > 0){
            for(int forint = 0; forint<varLongOut.size(); forint++){
              fragmentData(varLongOut.at(forint), false, false);
              clientTemp.write(md);
              clientTemp.write(ld);
            }
           }

           clientTemp.write(98);

           clientTemp.flush();



         //}//END while(sincroNotCompleted)

    } //END if(bufferSerial[0]=='c')

    else if(bufferSerial[0]=='f'){
      clientTemp.flush();
      configStream = true;
    }


  }//END if(clientTemp.available()>0)
  }//END if(!configStream)


  else{

    //txStream = true; //false;
    rxStream = false;//true;
    dataChecked = 0;
    int tempMultipli = 0;

   //ARDUINO to ANDROID

    cleanClientData(serTemp);


      serTemp.write(100); //KBCRA
      serTemp.write('g');

            fragmentData(inPS.size(),false,false);
            serTemp.write(md);
            serTemp.write(ld);

            serTemp.write('i');

             if(inPS.size()>0){                                     //Pin Entrada

              byteData = 0; //reset byteData
              j= 0;
              int multi = 1;
              tempMultipli = ((6*multi)-1);
              getInPins();

              for(int forint = 0; forint<inPS.size(); forint++){

                boolean dataTemp = inPS.at(forint);

                if(dataTemp){
                  byteData = bitMasks.at(j) | byteData;
                }

                j++;

                if(forint == tempMultipli){
                  j = 0;
                  serTemp.write(byteData);

                  byteData = 0;
                  multi++;
                  tempMultipli = ((6*multi)-1);
                }

              }//END for(forint = 0)

              if(j<6){  //There still been a byte to send
                serTemp.write(byteData);

              }

           }//END if(inPS.size)




           fragmentData(outPS.size(),false,false);
            serTemp.write(md);
            serTemp.write(ld);

            serTemp.write('o');

            if(outPS.size()>0){                                     //Pin Salida

              byteData = 0; //reset byteData
              j= 0;
              int multi = 1;
              tempMultipli = ((6*multi)-1);
              getOutPins();

              for(int forint = 0; forint<outPS.size(); forint++){

                boolean dataTemp = outPS.at(forint);

                if(dataTemp){
                  byteData = bitMasks.at(j) | byteData;
                }

                j++;

                if(forint == tempMultipli){
                  j = 0;
                  serTemp.write(byteData);
                  byteData = 0;
                  multi++;
                  tempMultipli = ((6*multi)-1);
                }

              }//END for(forint = 0)

              if(j<6){  //There still been a byte to send
                serTemp.write(byteData);
              }

            }//END if(outPS.size)




            fragmentData(varOutV.size(),false,false);
            serTemp.write(md);
            serTemp.write(ld);

            serTemp.write('v');

            if(varOutV.size()>0){                                     //Variable Salida


              for(int forint = 0; forint<varOutV.size(); forint++){
                int dataTemp = *varOutV.at(forint);
                fragmentData(dataTemp);
                serTemp.write(hd);
                serTemp.write(md);
                serTemp.write(ld);

              }//END for(forint = 0)
            }//END if(varOutV.size)



            fragmentData(logicOutV.size(),false,false);
            serTemp.write(md);
            serTemp.write(ld);

            serTemp.write('l');

            if(logicOutV.size()>0){                                     //Logico Salida

              byteData = 0; //reset byteData
              j= 0;
              int multi = 1;
              tempMultipli = ((6*multi)-1);


              for(int forint = 0; forint<logicOutV.size(); forint++){

                boolean dataTemp = *logicOutV.at(forint);

                if(dataTemp){
                  byteData = bitMasks.at(j) | byteData;
                }

                j++;

                if(forint == tempMultipli){
                  j = 0;
                  serTemp.write(byteData);
                  byteData = 0;
                  multi++;
                  tempMultipli = ((6*multi)-1);
                }

              }//END for(forint = 0)

              if(j<6){  //There still been a byte to send
                serTemp.write(byteData);
              }

            }//END if(logicOutV.size)


            fragmentData(varLongOutV.size(), false, false);               //Long Salida
            serTemp.write(md);
            serTemp.write(ld);

            serTemp.write('r');

            if(varLongOutV.size()>0){
              for(int forint = 0; forint < varLongOutV.size(); forint++){
                uint32_t dataTemp = *varLongOutV.at(forint);
                fragmentData(dataTemp, (uint8_t)4);
                serTemp.write(hd32);
                serTemp.write(md32);
                serTemp.write(ld32);
                serTemp.write(hd);
                serTemp.write(md);
                serTemp.write(ld);
              }

            }

            fragmentData(varFloatOutV.size(), false, false);
            serTemp.write(md);
            serTemp.write(ld);

            serTemp.write('p');

            if(varFloatOutV.size()>0){
              for(int forint = 0; forint < varFloatOutV.size(); forint++){
                floatValue tempFloat;
                tempFloat.val = *varFloatOutV.at(forint);

                uint32_t tempUint32 = (uint32_t)tempFloat.bytes[0];
                tempUint32 |= ((uint32_t)tempFloat.bytes[1] << 8);
                tempUint32 |= ((uint32_t)tempFloat.bytes[2] << 16);
                tempUint32 |= ((uint32_t)tempFloat.bytes[3] << 24);

                fragmentData(tempUint32, (uint8_t)4);
                serTemp.write(hd32);
                serTemp.write(md32);
                serTemp.write(ld32);
                serTemp.write(hd);
                serTemp.write(md);
                serTemp.write(ld);


              }
            }


      serTemp.write(98);  //DELIMITADOR


      dataChecked = buffSize = readMyByteUntil(98, serTemp);

      if(dataChecked > 0){
        if(bufferSerial[0] = 'f'){
          rxStream = true;
          cleanClientData(serTemp);
        }
      }
      

    if(rxStream){


    serTemp.write(100); //KBCRA
    serTemp.write('h');
    serTemp.write('h');
    serTemp.write(98);


    while(rxStream){



      if(serTemp.available()){
      

        dataChecked = buffSize = readMyByteUntil(98,serTemp);

        if(dataChecked>0){


          switch(bufferSerial[0]){

            case 'o':
              sizeIncome=joinInt8(bufferSerial[1], bufferSerial[2]);
              cursor = 3;

              if(sizeIncome > 0){
                if(sizeIncome ==  outPS.size()){

                  uint8_t tempMd2 = bufferSerial[cursor];
                  cursor++;
                  uint8_t tempLd2 = bufferSerial[cursor];
                  cursor++;

                  otherData = joinInt8(tempMd2, tempLd2);


    
                  outPS.clear();
                  j = 0;
                  int multi = 1;
                  tempMultipli = ((8*multi)-1);


                  for(int forint = 0; forint < sizeIncome; forint++){
                    int dataTemp = (bitMasks.at(j) & otherData);


                    if(dataTemp !=0) {
                      outPS.push_back(true);
                      writeOutPin(forint,true);
                    }
                    else {
                      outPS.push_back(false);
                      writeOutPin(forint,false);
                    }

                    j++;

                    if((forint == tempMultipli) && (forint != (sizeIncome-1))){
        

                      uint8_t tempMd2 = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempLd2 = bufferSerial[cursor];
                      cursor++;

                      otherData = joinInt8(tempMd2, tempLd2);

                      j=0;
                      multi++;
                      tempMultipli = ((8*multi)-1);
                    }//END if()

                  } //END for(int forint = 0; forint < sizeIncome; forint++)
                  cursor++;
                }//END if(sizeIncome == outPS.size())
              } //END if(sizeIncome > 0)

              serTemp.write(100);
              serTemp.write('h');
              serTemp.write('j');
              serTemp.write(98);

                                                                                      
              break;

            case 'e':
         
              sizeIncome=joinInt8(bufferSerial[1], bufferSerial[2]);
              cursor = 3;
      
              if(sizeIncome > 0){
                if(sizeIncome ==  logicInV.size()){

                  uint8_t tempMd2 = bufferSerial[cursor];
                  cursor++;
                  uint8_t tempLd2 = bufferSerial[cursor];
                  cursor++;

                  otherData = joinInt8(tempMd2, tempLd2);


                  j = 0;
                  int multi = 1;
                  tempMultipli = ((8*multi)-1);

                  for(int forint = 0; forint < sizeIncome; forint++){
                    int dataTemp = (bitMasks.at(j) & otherData);

                    if(dataTemp !=0) *logicInV.at(forint)= true;
                    else *logicInV.at(forint)=false;

                    j++;

                    if((forint == tempMultipli) && (forint != (sizeIncome-1))){

                      uint8_t tempMd2 = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempLd2 = bufferSerial[cursor];
                      cursor++;

                      otherData = joinInt8(tempMd2, tempLd2);

      
                      j=0;
                      multi++;
                      tempMultipli = ((8*multi)-1);
                    }//END if()

                  } //END for(int forint = 0; forint < sizeIncome; forint++)
                  cursor++;
                }//END if(sizeIncome == outPS.size())
              } //END if(sizeIncome > 0)

              serTemp.write(100);
              serTemp.write('h');
              serTemp.write('k');
              serTemp.write(98);

              break;

            case 'a':
           
              sizeIncome=joinInt8(bufferSerial[1], bufferSerial[2]);
              cursor = 3;

              if(sizeIncome > 0){
                if(sizeIncome ==  varInV.size()){
                  for(int forint =0; forint < sizeIncome; forint++){
                    uint8_t tempHd2 = bufferSerial[cursor];
                    cursor++;
                    uint8_t tempMd2 = bufferSerial[cursor];
                    cursor++;
                    uint8_t tempLd2 = bufferSerial[cursor];
                    cursor++;

                    *varInV.at(forint) = joinInt16(tempHd2, tempMd2, tempLd2);

                  }//END for(int forint =0; forint < sizeIncome; forint++)
                }//END if(sizeIncome ==  varInV.size())
              }//END if(sizeIncome > 0)

              serTemp.write(100);
              serTemp.write('h');
              serTemp.write('t');
              serTemp.write(98);


              break;


              case 'q':                                             //long input

                sizeIncome=joinInt8(bufferSerial[1], bufferSerial[2]);
                cursor = 3;

                if(sizeIncome > 0){
                  if(sizeIncome ==  varLongInV.size()){
                    for(int forint =0; forint < sizeIncome; forint++){
                      uint8_t tempHd2 = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempMd2 = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempLd2 = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempHd = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempMd = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempLd = bufferSerial[cursor];
                      cursor++;

                      long tempLong = joinInt32(tempHd2, tempMd2, tempLd2, tempHd, tempMd, tempLd);
                  
                      *varLongInV.at(forint) = tempLong;

                    }//END for(int forint =0; forint < sizeIncome; forint++)
                  }//END if(sizeIncome ==  varInV.size())
                }//END if(sizeIncome > 0)

                serTemp.write(100);
                serTemp.write('h');
                serTemp.write('x');
                serTemp.write(98);  
              break;


              case 'n':
                sizeIncome=joinInt8(bufferSerial[1], bufferSerial[2]);
                cursor = 3;

                if(sizeIncome > 0){
                  if(sizeIncome ==  varFloatInV.size()){
                    for(int forint =0; forint < sizeIncome; forint++){
                      uint8_t tempHd2 = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempMd2 = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempLd2 = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempHd = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempMd = bufferSerial[cursor];
                      cursor++;
                      uint8_t tempLd = bufferSerial[cursor];
                      cursor++;
 
                      uint32_t tempUint32 = joinInt32(tempHd2, tempMd2, tempLd2, tempHd, tempMd, tempLd);
                      float tempFloat = joinFloat(tempUint32);
                      *varFloatInV.at(forint) = tempFloat;

                    }//END for(int forint =0; forint < sizeIncome; forint++)
                  }//END if(sizeIncome ==  varInV.size())
                }//END if(sizeIncome > 0)


                rxStream = false;

              break;

            case 'm':
                rxStream = false;
              break;

            default:
                rxStream = false;
              break;

          }//END switch(bufferSerial)
        }//END if(sincroChecked)

      }//END if(clientTemp.available())

    }//END while(rxStream)




  }


  }//END if(configStream)



  }//end sincroEthernet


  Hmi_ Hmi;
