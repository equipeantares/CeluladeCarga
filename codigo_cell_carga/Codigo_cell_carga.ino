/*
 Example using the SparkFun HX711 breakout board with a scale
 By: Nathan Seidle
 SparkFun Electronics
 Date: November 19th, 2014
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

 This example demonstrates basic scale output. See the calibration sketch to get the calibration_factor for your
 specific load cell setup.

 This example code uses bogde's excellent library:"https://github.com/bogde/HX711"
 bogde's library is released under a GNU GENERAL PUBLIC LICENSE

 The HX711 does one thing well: read load cells. The breakout board is compatible with any wheat-stone bridge
 based load cell which should allow a user to measure everything from a few grams to tens of tons.
 Arduino pin 2 -> HX711 CLK
 3 -> DAT
 5V -> VCC
 GND -> GND

 The HX711 board can be powered from 2.7V to 5V so the Arduino 5V power should be fine.

*/

#include "HX711.h"
#include <SD.h>
#include <SPI.h>
#include <stdio.h>
#include <SoftwareSerial.h>                              //Biblioteca de comunicação do módulo
#include <EBYTE.h>                                       //Biblioteca do Kris

String urlMapa;
float flon, flat;


#define M0_LoRa   11
#define M1_LoRa   12
#define RX_LoRa   3                                     // Vai no TXD do módulo
#define TX_LoRa   2                                     // Vai no RXD do módulo
#define AUX_LoRa  4
                                                      
SoftwareSerial lora(RX_LoRa, TX_LoRa);                  //Definição dos pinos para criar a comunicação serial
EBYTE LoRa(&lora, M0_LoRa, M1_LoRa, AUX_LoRa);          //Criar objeto de Transmissão, passando na comunicação serial e pinos

#define SD_CS_PIN 10 // Pin onde se conecta pino CS do leitor SD

#define calibration_factor 870 //Fator de calibração obtido previamente em outro código
#define LOADCELL_DOUT_PIN  A1
#define LOADCELL_SCK_PIN  A0

char contagem = 0;
HX711 scale;
File dataFrameCelula;

void write_on_SD(float dado){
  dataFrameCelula.print(dado);
  dataFrameCelula.print(" N");
  dataFrameCelula.print("\n");
}

void setup() {
  Serial.begin(9600);

  while (!Serial);

  lora.begin(9600);
  LoRa.init();

  // LoRa.Reset(); // Opcional: Reseta os parâmetros para os de fábrica

  // Defina a taxa de dados de acordo com suas necessidades
  LoRa.SetAirDataRate(ADR_1K);

  // Defina o endereço da rede
  LoRa.SetAddress(1);

  // Defina o canal de comunicação
  LoRa.SetChannel(23);

  // Salve as configurações
  LoRa.SaveParameters(TEMPORARY);

  // Imprima os parâmetros do módulo
  LoRa.PrintParameters();

  // Defina o modo de operação
  LoRa.SetMode(MODE_NORMAL);


  Serial.println("HX711 scale demo");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0

  Serial.println("Readings:");
}
void taragem(){
  if (Serial.available()){
    char cara = Serial.read();
    if(cara == 't')
      scale.tare();
  }
  }
void loop() {
  float dado;
  if (lora.readString() == "ler") {
    Serial.print("Reading: ");
    dado = scale.get_units();
    Serial.print(dado, 3); //scale.get_units() returns a float
    Serial.print(" N"); //You can change this to kg but you'll need to refactor the calibration_factor
    write_on_SD(dado);
    Serial.println();
    taragem();
    lora.print(dado);
  }
}
