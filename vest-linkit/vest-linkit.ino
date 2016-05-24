#include <Wire.h>
String message = "";
String inString = "";
// 1430784000

int t1;
int t2;
int t3;
int t4;
int t5;


int ledpin = 14;
int tempPin = 15;
int ts = 23;

//date
#include <Time.h>
#include <LDateTime.h>
datetimeInfo t;
unsigned int rtc;
#include <LGPRSUdp.h>

unsigned int localPort = 2390;      // local port to listen for UDP packets

#define TIME_SERVER "time-c.nist.gov"  // a list of NTP servers: http://tf.nist.gov/tf-cgi/servers.cgi

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A LGPRSUDP instance to let us send and receive packets over UDP with LinkIt
LGPRSUDP Udp;

/// gprs
#include <LGPRS.h>
#include <LGPRSClient.h>
#include <LGPRSServer.h>
//#include <SPI.h>
//char server[] = "";
char server[] = "192.168.1.237";
//char server[] = "";
char path[] = "/linkit-data";
int port = 5000; // HTTP



/// Wifi

#include <LTask.h>
#include <LWiFi.h>
#include <LWiFiClient.h>
#include <LBattery.h>

#define WIFI_AP ""
#define WIFI_PASSWORD ""
#define WIFI_AUTH LWIFI_WPA  // choose from LWIFI_OPEN, LWIFI_WPA, or LWIFI_WEP.
int counter = 0;
int PostCounter = 0;

LGPRSClient gprsClient;
LWiFiClient wifiClient;



///sd


#include <LTask.h>
#include <LFlash.h>
#include <LSD.h>
#include <LStorage.h>

#define Drv LFlash          // use Internal 10M Flash
// #define Drv LSD           // use SD card

LFile myFile;

char* fileNameC = "undefined.txt";
String fileNameS = "undefined.txt";

void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(115200);  // start serial for output
  /// gprs
  while (!LGPRS.attachGPRS("internet", "eesecure", "secure"))
  {
    delay(500);
  }
  //Client has to be initiated after GPRS is established with the correct APN settings
  LGPRSClient client;
  /// date
  //  delay(10000);
  //  while (!Udp.begin(localPort))
  //  {
  //    Serial.println("retry begin");
  //    delay(1000);
  //  }
  //  date();
  /// wifi
  LTask.begin();
  LWiFi.begin();
  //sd
  Drv.begin();


  //  Serial.println("debug no arduino");
  //  t1 = 21;
  //  t2 = 22;
  //  t3 = 23;
  //  t4 = 24;
  //  t5 = 25;

  Serial.println("setup() done");
}
int WriteCounter = 0;
void loop() {
  serialHandler();
  /// request temperatures

  wire();

  /// write to sd
  if (PostCounter == 4) {
    writeSD("log");
    PostCounter = 0;
  } else {
    PostCounter++;
  }

  delay(1000);
}
String dateFormatS(String type) {
  LDateTime.getTime(&t);
  String dates;
  if (type == "json") {
    char date[22];
    sprintf(date,  "%d-%d-%dT%d:%d:%d.json", t.year, t.mon, t.day, t.hour, t.min, t.sec);
    dates =   String(date);
  } else if (type == "txt") {
    char date[21];
    sprintf(date,  "%d-%d-%dT%d:%d:%d.txt", t.year, t.mon, t.day, t.hour, t.min, t.sec);
    dates =   String(date);
  } else {
    char date[17];
    sprintf(date,  "%d-%d-%dT%d:%d:%d", t.year, t.mon, t.day, t.hour, t.min, t.sec);
    dates =   String(date);
  }

  return dates;
}
void SetFileName(String type) {
  LDateTime.getTime(&t);
  if (type == "json") {
    char date[22];
    sprintf(date,  "%d-%d-%dT%d:%d:%d.json", t.year, t.mon, t.day, t.hour, t.min, t.sec);
    fileNameC = date;
  } else if (type == "txt") {
    char date[21];
    sprintf(date,  "%d-%d-%dT%d:%d:%d.txt", t.year, t.mon, t.day, t.hour, t.min, t.sec);
    fileNameC = date;
  } else {
    char date[17];
    sprintf(date,  "%d-%d-%dT%d:%d:%d", t.year, t.mon, t.day, t.hour, t.min, t.sec);
  }
}

//String fileNameS() {
//  String fileNameS;
//  /// todo: list files;  read last; if hasn't got posted at the end use the file if not create new from date
//  return fileNameS;
//}

void serialHandler() {
  if (Serial.available()) {
    String  msS;

    while (Serial.available()) {
      char rea = Serial.read();
      msS += rea;
    }

    Serial.println(msS);
    if (msS == "r") {
      String datser = readSD();
      //   datser.trim();datser.trim();datser.trim();datser.trim();datser.trim();



      datser.trim();
      int   sz = datser.length();

      Serial.print(datser);
      //      Serial.println(lastOpeningBracket);
      datser = "";
    } else if (msS == "rs") {
      int flsize = getFileSize();
      Serial.println(flsize);
      flsize = 0;
    } else if (msS == "p") {
      Post();
    }
    else if (msS == "pt") {
      PostTest();
    } else if (msS == "d") {


      Serial.println(dateFormatS("json"));
    }
    msS = "";
  }
}
void Post() {
  bool wifiCon = LWiFi.connect(WIFI_AP, LWiFiLoginInfo(WIFI_AUTH, WIFI_PASSWORD));
  delay(1000);

  LGPRSClient c;
  if (wifiCon) {
    LWiFiClient c;
    Serial.println("wifi con");
  }


  //  LGPRSClient c;
  // if there are incoming bytes available
  // from the server, read them and print them:
  String data;
  while (!c.connect(server, port))
  {
    Serial.println("Retrying to connect...");
    delay(100);
  }
  if (c.connected()) {
    //read data
    String dat =  readSD();
    int size = dat.length() + 2;
    //    int size = getFileSize();


    // Build HTTP POST request
    c.print("POST "); c.print(path); c.println(" HTTP/1.1");
    c.print("Host: "); c.print(server); c.print(":"); c.println(port); // localhost if running localy
    c.println("Connection: close");
    c.println("Content-Type: application/json");
    c.print("Content-Length: ");
    c.println(size);
    c.println();
    c.print("[");
    c.print(dat);
    c.println("]");
    c.println();

    //    Serial.println(dat);

    writeSD("Posted");
    SetFileName("txt");

  } else {
    Serial.println("Client disconnected.");
  }


  // wait to fully let the client disconnect
  delay(3000);
}
void PostTest() {
  bool wifiCon = LWiFi.connect(WIFI_AP, LWiFiLoginInfo(WIFI_AUTH, WIFI_PASSWORD));
  delay(1000);
  Serial.println(" PostTest");

  LGPRSClient c;
  if (wifiCon) {
    LWiFiClient c;
    Serial.println("wifi con");
  }


  //  LGPRSClient c;
  // if there are incoming bytes available
  // from the server, read them and print them:
  String data;
  while (!c.connect(server, port))
  {
    Serial.println("Retrying to connect...");
    delay(100);
  }
  if (c.connected()) {
    //read data
    String dat =  readSD();
    int size = getFileSize();


    // String dat = "[{\"value\":\"5dfg888282gdfgdgdfgdfg8\"},{\"value\":\"5888dfbgdfg282gdfgdfg8\"},]";
    // int size = 4dat.length();


    // Build HTTP POST request

    c.print("POST "); c.print(path); c.println(" HTTP/1.1");
    c.print("Host: "); c.print(server); c.print(":"); c.println(port); // localhost if running localy
    c.println("Connection: close");
    c.println("Content-Type: application/json");
    c.print("Content-Length: ");
    c.println(size);
    c.println();
    c.print("[");
    c.print(dat);
    c.println("]");
    c.println();
    Serial.println(dat);

  } else {
    Serial.println("Client disconnected.");
  }
  Serial.println("Client disconnected.");

  // wait to fully let the client disconnect
  delay(3000);
}

int getFileSize() {
  int size;
  myFile = Drv.open(fileNameC);
  if (myFile) {
    size = myFile.size();
    Serial.print(fileNameS); Serial.print(" size: "); Serial.println(size);
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.print("error opening "); Serial.println(fileNameS);
  }
  return size;
}
String readSD() {
  String dat;

  myFile = Drv.open(fileNameC);
  if (myFile) {
    myFile.seek(0);

    while (myFile.available()) {
      char r = myFile.read();
      dat += r;
    }

    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.print("error opening "); Serial.println(fileNameS);
  }
  int lastcommma = dat.lastIndexOf(',');
  dat.setCharAt(lastcommma, ' ');
  dat.trim();
  return dat;
}
void writeSD (String type) {
  /// get date
  LDateTime.getTime(&t);
  //  int* getDateInt = getDateInt();
  char dataArr[100];
  myFile = Drv.open(fileNameC, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    if (type == "log") {
      sprintf(dataArr,  "{\"t1\":\"%d\",\"t2\":\"%d\",\"t3\":\"%d\",\"t4\":\"%d\",\"t5\":\"%d\",\"ts\":\"%d\",\"ti\":\"%d-%d-%dT%d:%d:%d\"},", t1, t2, t3, t4, t5, ts, t.year, t.mon, t.day, t.hour, t.min, t.sec);

      myFile.println(dataArr );
      // close the file:
      myFile.close();
      Serial.println("writeSD () done.");
    } else {
      myFile.println(type);
    }


  } else {
    // if the file didn't open, print an error:
    Serial.print("error opening test.txt"); Serial.println(fileNameS);
  }
}
void wire() {
  Wire.requestFrom(8, 5);    // request 6 bytes from slave device #8

  while (Wire.available()) { // slave may send less than requested
    char c = Wire.read(); // receive a byte as character
    //    Serial.print(c);         // print the character
    message += c;
    if (isDigit(c)) {
      // convert the incoming byte to a char
      // and add it to the string:
      inString += (char)c;
    }
  }
  int  inInt = inString.toInt();
  if (message.substring(3) == "send") {
    Post();
  } else if (message.substring(3) == "tz") {
    t1 = inInt;
  }
  else if (message.substring(3) == "tl") {
    t2 = inInt;
  }
  else if (message.substring(3) == "tb") {
    t3 = inInt;
  }
  else if (message.substring(3) == "ta") {
    t4 = inInt;
  }
  else if (message.substring(3) == "tr") {
    t5 = inInt;
  }
  else if (message.substring(3) == "ts") {
    ts = inInt;
  }
  else {
    Serial.println("err msg");

  }
  inString = "";
  message = "";
}

void date() {
  sendNTPpacket(); // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(1000);
  Serial.println( Udp.parsePacket() );
  if ( Udp.parsePacket() ) {
    Serial.println("packet received");
    // We've received a packet, read the data from it
    memset(packetBuffer, 0xcd, NTP_PACKET_SIZE);
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
    for (int i = 0; i < NTP_PACKET_SIZE; ++i)
    {
      Serial.print(packetBuffer[i], HEX);
    }
    Serial.println();


    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    Serial.print("Seconds since Jan 1 1900 = " );
    Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    // print Unix time:
    Serial.println(epoch);

    int yeari = year(epoch);
    int moni = month(epoch);
    int dayi = day(epoch);
    int houri = hour(epoch);
    int mini = minute(epoch);
    int seci = second(epoch);
    t.year = yeari;
    t.mon = moni;
    t.day = dayi;
    t.hour = houri;
    t.min = mini;
    t.sec = seci;
    LDateTime.setTime(&t);
  }
}
// send an NTP request to the time server at the given address
unsigned long sendNTPpacket() {
  Serial.println("sendNTPpacket");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  //Serial.println("2");
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(TIME_SERVER, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

