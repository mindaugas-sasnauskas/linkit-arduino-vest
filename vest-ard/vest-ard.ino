#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
char* filename = "data1.txt";
// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 9
int msg_tmp;
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
bool send2web = false;
int ct = 1; // wire counter

int t1;
int t2;
int t3;
int t4;
int t5;

int ledpin = 14;
int tempPinR = 2;
int tempPinL = 3;
int tempPinM = 9;
int ts = 23;

long int timestamp;
char dataArr[100];
// bt

SoftwareSerial BTserial(10, 11); // TX, RX

#define MAX_BUFFER 4

char data;
char* buffer;
boolean receiving = false;
int pos;

void setup()  {
  Serial.begin(115200);
  // bt
  BTserial.begin(9600);
  buffer = new char[MAX_BUFFER];
  // END bt
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
  pinMode(tempPinR, OUTPUT);
  pinMode(tempPinL, OUTPUT);
  pinMode(tempPinM, OUTPUT);
  pinMode(ledpin, OUTPUT);
  // Start up the library
  sensors.begin();


}

void loop()  {
  sensors.requestTemperatures(); // Send the command to get temperatures
  t1 = sensors.getTempCByIndex(0);
  t2 = sensors.getTempCByIndex(1);
  t3 = sensors.getTempCByIndex(2);
  t4 = sensors.getTempCByIndex(3);
  t5 = sensors.getTempCByIndex(4);

  //  SimulateRequestTemp();
  delay(500);
  vestTempControl();
  bt();
  Serial.print("send2web: "); Serial.println(send2web);
}

void RequestTemp() {
  sensors.requestTemperatures(); // Send the command to get temperatures
  t1 = sensors.getTempCByIndex(0);
  t2 = sensors.getTempCByIndex(1);
  t3 = sensors.getTempCByIndex(2);
  t4 = sensors.getTempCByIndex(3);
  t5 = sensors.getTempCByIndex(4);
}
int sim_it = 0;
void SimulateRequestTemp() {
  ///debug no arduino
  t1 = 21;
  t2 = 22;
  t3 = 23;
  t4 = 24;
  t5 = 25;
  int rand_n;
  if (sim_it == 1) {
    rand_n = 1;
    t1 = t1 + rand_n;
    t2 = t2 + rand_n;
    t3 = t3 + rand_n;
    t4 = t4 + rand_n;
    t5 = t5 + rand_n;

  } else if (sim_it == 2) {
    rand_n = 2;
    t1 = t1 + rand_n;
    t2 = t2 + rand_n;
    t3 = t3 + rand_n;
    t4 = t4 + rand_n;
    t5 = t5 + rand_n;
  } else if (sim_it == 3) {
    rand_n = 3;
    t1 = t1 + rand_n;
    t2 = t2 + rand_n;
    t3 = t3 + rand_n;
    t4 = t4 + rand_n;
    t5 = t5 + rand_n;
  } else if (sim_it == 4) {
    rand_n = 4;
    t1 = t1 + rand_n;
    t2 = t2 + rand_n;
    t3 = t3 + rand_n;
    t4 = t4 + rand_n;
    t5 = t5 + rand_n;
  } else if (sim_it == 5) {
    rand_n = 5;
    t1 = t1 + rand_n;
    t2 = t2 + rand_n;
    t3 = t3 + rand_n;
    t4 = t4 + rand_n;
    t5 = t5 + rand_n;
    sim_it += 0;
  }
  sim_it++;

}

void bt()
{
  if (BTserial.available()) {
    data = BTserial.read();
    int tss;
    switch (data) {
      //3: End of transmission
      case 3:  receiving = false;
        msg_tmp = buffer2int(buffer);
        if (msg_tmp > 10 && msg_tmp <= 55) {
          ts = msg_tmp;
        } else if ( msg_tmp == 2) {
          send2web = true;
        }
        btWrite();
        Serial.print("ts:  "); Serial.println(ts);
        break; //end message
      default: if (receiving == false) resetData();
        buffer[pos] = data;
        pos++;
        receiving = true;
    }
    delay(10);

  }
}
void btWrite() {
  sprintf(dataArr, "{\"data\":[{\"t1\":\"%d\",\"t2\":\"%d\",\"t3\":\"%d\",\"t4\":\"%d\",\"t5\":\"%d\",\"ts\":\"%d\"}]}|", t1, t2, t3, t4, t5, ts);

  Serial.println(dataArr);
  BTserial.print(dataArr);
}
void resetData() {
  for (int i = 0; i <= pos; i++) buffer[i] = 0;
  pos = 0;
}

int buffer2int(char* buffer) {
  int i;
  sscanf(buffer, "%d", &i);
  return i;

}


void requestEvent() {
  char* msg = "00000";
  if (send2web) {
    Wire.write("send");
    send2web = 0;
  } else {
    switch (ct) {
      case 1:
        sprintf(msg, "%d tz", t1);
        ct = 2;
        Wire.write(msg);
        break;
      case 2:
        sprintf(msg, "%d tl", t2);
        ct = 3;
        Wire.write(msg);
        break;
      case 3:
        sprintf(msg, "%d tb", t3);
        ct = 4;
        Wire.write(msg);
        break;
      case 4:
        sprintf(msg, "%d ta", t4);
        ct = 5;
        Wire.write(msg);
        break;
      case 5:
        sprintf(msg, "%d tr", t5);
        ct = 6;
        Wire.write(msg);
        break;
      case 6:
        sprintf(msg, "%d ts", ts);
        ct = 1;
        Wire.write(msg);
        break;
      default:
        Wire.write("defal");
        ct = 1;
        break;
    }
  }
}

void vestTempControl() {
  Serial.print("ts "); Serial.println(ts);
  Serial.print("t1 "); Serial.println(t1);
  Serial.print("t2 "); Serial.println(t2);
  //// left
  if (t1 < ts ) {
    digitalWrite(tempPinL, HIGH);
    Serial.print(" tempPinL "); Serial.println("on");
  } else  {
    digitalWrite(tempPinL, LOW);
    Serial.print(" tempPinL "); Serial.println("off");
  }
  //// right
  if (t2 < ts) {
    digitalWrite(tempPinR, HIGH);
    Serial.print("tempPinR "); Serial.println("on");
  } else  {
    digitalWrite(tempPinR, LOW);
    Serial.print(" tempPinR "); Serial.println("off");
  }
  //  if (t1 < ts) {
  //    digitalWrite(tempPinM, HIGH);
  //    Serial.print("tempPinL ");Serial.println("on");
  //  } else  {
  //    digitalWrite(tempPinM, LOW);
  //  }

}
