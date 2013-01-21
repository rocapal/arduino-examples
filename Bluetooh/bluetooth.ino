/*
 *
 *  Copyright (C) Roberto Calvo Palomino
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see http://www.gnu.org/licenses/. 
 *
 *  Author : Roberto Calvo Palomino <rocapal at gmail dot com>
 *
 */
 
int LED = 8;         // LED pin
int DoorPin = 0;     // Door open sensor


void blink();

void setup()
{
  
  pinMode(LED, OUTPUT);
  pinMode(DoorPin, INPUT);
  
  digitalWrite(LED, LOW);  
  
  Serial.begin(9600);
  
  blink();
}

void blink()
{
  digitalWrite(LED, HIGH);
  delay(1000);
  digitalWrite(LED, LOW);
  delay(1000);
  digitalWrite(LED, HIGH);
  delay(1000);
  digitalWrite(LED, LOW);
  delay(1000);
}



void loop()
{ 
  
  while (Serial.available()  > 0 ){ 
    // read the oldest byte in the serial buffer:
    byte incomingByte = Serial.read();
    // if it's a capital R, reset the counter
    if (incomingByte == 'L') { // Led 
      blink();
    }
    else if (incomingByte == 'D') { // Door sensor
      int doorSensor = analogRead(DoorPin);
      byte response = (doorSensor < 1000 ? 0 : 1 ); // 0: Open Door; 1: Close Door
      Serial.write(response);
    }
  
  }
  
  delay(500);
  
}
