/*
  SD card read/write

 This example shows how to read and write data to and from an SD card file
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

 created   Nov 2010
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe

 This example code is in the public domain.

 */
#ifndef gravar_h
#define gravar_h

#include <SPI.h>
#include <SD.h>
#include "fila_.h"
File myFile;
ISR (PCINT2_vect)
{
    // if the file opened okay, write to it:
    if(!myFile){
      myFile = SD.open("newtest.txt", FILE_WRITE);
    }
    if (myFile) {
      //Serial.println("Writing to test.txt..."); 
      //int inicio  = micros();
       myFile.write(fila,256);
       //Serial.println("tamanho");
       //Serial.println(tamanho);
      //int fim = micros();
      //Serial.println(fim-inicio);
      //Serial.println("Interrupcao");
     // myFile.flush();
    }
    else {
      // if the file didn't open, print an error:
      //Serial.println("error opening test.txt");
    }
       
}
#endif gravar_h
