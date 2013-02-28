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


#include <SPI.h>
#include <Ethernet.h>

static const int LedPin =8;
static const int SwitchPin =7;

#define STRING_BUFFER_SIZE 128
char buffer[STRING_BUFFER_SIZE];

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x00, 0x5E, 0xCB };
IPAddress ip(192,168,1,10);

EthernetServer server(80);


void setup()
{
  Serial.begin(9600);
 
  pinMode(LedPin, OUTPUT); 
  pinMode(SwitchPin, INPUT); 
  
  Serial.println("pre-power");
  Serial.println("pro-power");
  
  
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  
  
  delay (2000); 
}

int readSwitch()
{

  return digitalRead(SwitchPin);

}

void sendError(EthernetClient client) {
     client.println("HTTP/1.1 404 OK");
     client.println("Content-Type: text/json");
     client.println("Connnection: close");
     client.println();
     client.println("{ \"msg\" : \"Command not recognized\"}");
}

void sendStatus (EthernetClient client)
{
  // send a standard http response header          
  int switchStatus = readSwitch();
          
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/json");
  client.println();

  client.print("{");
  client.print("\"switch\":");
  client.print(switchStatus);
  client.print(", \"temp\": 25.40");
  client.println("}");
}

void sendStatusLed (EthernetClient client, int ledStatus)
{

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/json");
  client.println();
  
  client.print("{");
  client.print("\"led\":");
  client.print(ledStatus);
  client.println("}");

}


void handleCommand(EthernetClient client, char* cmd, char* param) {
  
    Serial.println("Command & params");
    Serial.println(cmd);
    Serial.println(param);
    
    if (strcmp(cmd, "status") == 0) {
      Serial.println("status");
      sendStatus(client);
      return;   
    } 
    
    if (strcmp(cmd, "led") == 0) {
      if (strcmp(param, "on") == 0)
      {
        digitalWrite(LedPin, HIGH);
        sendStatusLed(client, HIGH);
        return;
      }
      else if (strcmp(param, "off") == 0)
      {
        digitalWrite(LedPin, LOW);
        sendStatusLed(client, LOW);
        return;
      }
     
    }
    
    sendError(client);

}

char** parse(char* str) {

    char ** messages;
    messages = (char**)malloc(sizeof(char *));
    char *p;
    p = strtok(str, " ");
    unsigned int i = 0;
    while (p != NULL) {
      p = strtok(NULL, "/");
      char *sp;
      boolean last = false;
      sp = strchr(p, ' ');
      if (sp != NULL) {
            *sp++ = '\0';
        last = true;
      }
      messages[i] = p;
      i++;
      if (last) {
        break;
      }
      messages = (char**)realloc(messages, sizeof(char *) * i + 1);
    }

    messages[i] = '\0';
    return messages;
}

int countSegments(char* str) {
  int p = 0;
  int count = 0;
  while (str[p] != '\0') {
    if (str[p] == '/') {
      count++;
    }
    p++;
  }
  // We don't want to count the / in 'HTTP/1.1'
  count--;
  return count;
}


void loop() {
 
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    
    while (client.connected()) {
      if (client.available()) {
        
        char c;
        int bufindex = 0; // reset buffer
        buffer[0] = client.read();
        buffer[1] = client.read();
        bufindex = 2;
        // Read the first line to determin the request page
        while (buffer[bufindex-2] != '\r' && buffer[bufindex-1] != '\n') { 
            // read full row and save it in buffer
            c = client.read();
            if (bufindex<STRING_BUFFER_SIZE) {
              buffer[bufindex] = c;
            }
            bufindex++;
        }
        // Clean buffer for next row
        bufindex = 0;
 
        // Parse the query string
        int nSegments = countSegments(buffer);
        char **pathsegments = parse(buffer);
 
        int i = 0;
        for(i=0; i<nSegments; i++) {
          Serial.println(pathsegments[i]);
        }

        if (c == '\n' && currentLineIsBlank) {
          handleCommand(client, pathsegments[0], pathsegments[1]);
          break;
        }

        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    
  
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}


