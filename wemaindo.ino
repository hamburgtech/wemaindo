#include <FastLED.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
// size of buffer used to capture HTTP requests
#define REQ_BUF_SZ   90
// size of buffer that stores the incoming string
#define TXT_BUF_SZ   50

// MAC address from Ethernet shield sticker under board
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 178, 150);   // IP address, may need to change depending on network
EthernetServer server(80);       // create a server at port 80
File webFile;                    // the web page file on the SD card
char HTTP_req[REQ_BUF_SZ] = {0}; // buffered HTTP request stored as null terminated string
char req_index = 0;              // index into HTTP_req buffer
char txt_buf[TXT_BUF_SZ] = {0};  // buffer to save text to

#define NUM_LEDS 121      // number of LEDs
#define DATA_PIN 6        // datapin for LEDs
#define CHIPSET WS2811    // used LED chip
CRGB leds[NUM_LEDS];

boolean dot = false;      //for Dot-Animation
int lead_dot = 0;         //for Dot-Animation

boolean rainbow = false;

void setup()
{
  FastLED.addLeds<CHIPSET, DATA_PIN, GRB>(leds, NUM_LEDS); //see doc for different LED strips, GRB sets other color calibration
    for(int y = 0 ; y < NUM_LEDS ; y++) 
  {
    leds[y] = CRGB::Black; // set all leds to black during setup
  }
  FastLED.show();
  
    // disable Ethernet chip
    pinMode(10, OUTPUT);
    digitalWrite(10, HIGH);
    
    Serial.begin(115200);       // for debugging
    
    // initialize SD card
    Serial.println("Initializing SD card...");
    if (!SD.begin(4)) {
        Serial.println("ERROR - SD card initialization failed!");
        return;    // init failed
    }
    Serial.println("SUCCESS - SD card initialized.");
    // check for index.htm file
    if (!SD.exists("index.htm")) {
        Serial.println("ERROR - Can't find index.htm file!");
        return;  // can't find index file
    }
    Serial.println("SUCCESS - Found index.htm file.");
    
    Ethernet.begin(mac, ip);  // initialize Ethernet device
    server.begin();           // start to listen for clients
}

void loop()
{
    EthernetClient client = server.available();  // try to get client

    if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                // limit the size of the stored received HTTP request
                // buffer first part of HTTP request in HTTP_req array (string)
                // leave last element in array as 0 to null terminate string (REQ_BUF_SZ - 1)
                if (req_index < (REQ_BUF_SZ - 1)) {
                    HTTP_req[req_index] = c;          // save HTTP request character
                    req_index++;
                }
                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                if (c == '\n' && currentLineIsBlank) {
                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    // remainder of header follows below, depending on if
                    // web page or XML page is requested
                    // Ajax request - send XML file
                    if (StrContains(HTTP_req, "ajax_inputs")) {
                        // send rest of HTTP header
                        client.println("Content-Type: text/xml");
                        client.println("Connection: keep-alive");
                        client.println();

                  
                        //Get LED number and value
                        
                     
                        if (GetText(txt_buf, TXT_BUF_SZ)) {
                        
                         String startCode = getValue(txt_buf, ';', 0); 
                          
                          if (strcmp (txt_buf,"alloff") == 0)
                          {
                            FastLED.clear();
                            FastLED.show();
                          }
                          else if(startCode=="runningdot")    //draw a running dot animation
                          {
                           if(dot==false){
                           dot=true;
                           
                          String redValueStr = getValue(txt_buf, ';', 1);  // ToDo create function!!!!
                          int redValueInt = redValueStr.toInt();
                          
                          String greenValueStr = getValue(txt_buf, ';', 2);
                          int greenValueInt = greenValueStr.toInt();
                          
                          String blueValueStr = getValue(txt_buf, ';', 3);
                          int blueValueInt = blueValueStr.toInt();
                          
                          dotWithTrail(redValueInt, greenValueInt, blueValueInt);
                           
                           }
                          }
                            else if(startCode=="rainbow")  {  //draw a rainbow animation
                            if(rainbow==false){
                            rainbow=true;
                            drawRainbow();
                            }
                            }
                            
                            else {  
                       
                           // String ledNumStr = getValue(txt_buf, ';', 0);              //ToDo: create getColor function
                            int ledNumInt = startCode.toInt();
                          
                          String redValueStr = getValue(txt_buf, ';', 1);
                          int redValueInt = redValueStr.toInt();
                          
                          String greenValueStr = getValue(txt_buf, ';', 2);
                          int greenValueInt = greenValueStr.toInt();
                          
                          String blueValueStr = getValue(txt_buf, ';', 3);
                          int blueValueInt = blueValueStr.toInt();
                                                    
                       
                          leds[ledNumInt-1].setRGB(redValueInt, greenValueInt, blueValueInt); // set colors of LEDs, FastLED library function
                          FastLED.show();
                         }
                        }
                            
                        }
                    
                            
                         //  fill_rainbow(leds, NUM_LEDS, 0, 10);       //build in FastLED function
                          // drawDot(); 
                           
                          // FastLED.show();
               
                    
                                                                
                        
                    
                    else {  // web page request
                        // send rest of HTTP header
                        client.println("Content-Type: text/html");
                        client.println("Connection: keep-alive");
                        client.println();
                        // send web page
                        webFile = SD.open("index.htm");        // open web page file
                        if (webFile) {
                            while(webFile.available()) {
                                client.write(webFile.read()); // send web page to client
                            }
                            webFile.close();
                        }
                    }
                    // reset buffer index and all buffer elements to 0
                    req_index = 0;
                    StrClear(HTTP_req, REQ_BUF_SZ);
                    break;
                }
                // every line of text received from the client ends with \r\n
                if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)
}

// extract text from the incoming HTTP GET data string
// returns true only if text was received
// the string must start with "&txt=" and end with "&end"
// if the string is too long for the HTTP_req buffer and
// "&end" is cut off, then the function returns false
boolean GetText(char *txt, int len)
{
  boolean got_text = false;    // text received flag
  char *str_begin;             // pointer to start of text
  char *str_end;               // pointer to end of text
  int str_len = 0;
  int txt_index = 0;
  
  // get pointer to the beginning of the text
  str_begin = strstr(HTTP_req, "&txt=");
  if (str_begin != NULL) {
    str_begin = strstr(str_begin, "=");  // skip to the =
    str_begin += 1;                      // skip over the =
    str_end = strstr(str_begin, "&end");
    if (str_end != NULL) {
      str_end[0] = 0;  // terminate the string
      str_len = strlen(str_begin);

      // copy the string to the txt buffer and replace %20 with space ' '
      for (int i = 0; i < str_len; i++) {
        if (str_begin[i] != '%') {
          if (str_begin[i] == 0) {
            // end of string
            break;
          }
          else {
            txt[txt_index++] = str_begin[i];
            if (txt_index >= (len - 1)) {
              // keep the output string within bounds
              break;
            }
          }
        }
        else {
          // replace %20 with a space
          if ((str_begin[i + 1] == '2') && (str_begin[i + 2] == '0')) {
            txt[txt_index++] = ' ';
            i += 2;
            if (txt_index >= (len - 1)) {
              // keep the output string within bounds
              break;
            }
          }
        }
      }
      // terminate the string
      txt[txt_index] = 0;
      got_text = true;
    }
  }

  return got_text;
}

// sets every element of str to 0 (clears array)
void StrClear(char *str, char length)
{
    for (int i = 0; i < length; i++) {
        str[i] = 0;
    }
}

// searches for the string sfind in the string str
// returns 1 if string found
// returns 0 if string not found
char StrContains(char *str, char *sfind)
{
    char found = 0;
    char index = 0;
    char len;

    len = strlen(str);
    
    if (strlen(sfind) > len) {
        return 0;
    }
    while (index < len) {
        if (str[index] == sfind[found]) {
            found++;
            if (strlen(sfind) == found) {
                return 1;
            }
        }
        else {
            found = 0;
        }
        index++;
    }

    return 0;
}

String getValue(String data, char separator, int index)
{
 int found = 0;
  int strIndex[] = {
0, -1  };
  int maxIndex = data.length()-1;
  for(int i=0; i<=maxIndex && found<=index; i++){
  if(data.charAt(i)==separator || i==maxIndex){
  found++;
  strIndex[0] = strIndex[1]+1;
  strIndex[1] = (i == maxIndex) ? i+1 : i;
  }
 }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}


void drawDot() 
{

  for(int dot = 0; dot < NUM_LEDS; dot++)
  { 
    
    leds[dot] = CRGB::Blue;
    FastLED.show();
    
    // clear this led for the next time around the loop
    leds[dot] = CRGB::Black;
    
    delay(30);
  }

  for(int dot = 0; dot < NUM_LEDS; dot++)            
  { 
    
    leds[NUM_LEDS - dot] = CRGB::Pink;
    FastLED.show();
    
    // clear this led for the next time around the loop
    leds[NUM_LEDS - dot] = CRGB::Black;
    
    delay(30);
  }
}


void dotWithTrail(int r, int g, int b)
{
while(dot==true){
EVERY_N_MILLISECONDS(50)
{

  for(int dot = 0; dot < NUM_LEDS; dot++)
  { 
  
    leds[dot].fadeToBlackBy(64);
    
  }
  
  leds[lead_dot].setRGB(r, g, b);
  
  lead_dot = lead_dot + 1;
  if(lead_dot == NUM_LEDS+1) { lead_dot = 0; dot = false; FastLED.clear(); FastLED.show(); }
  
  FastLED.show();
}
}
}

void drawRainbow(){                 // not working at the moment, need to be changed
//while(rainbow==true){

//fill_rainbow(leds, NUM_LEDS, 0, 5);
FastLED.show();  

//}
}
