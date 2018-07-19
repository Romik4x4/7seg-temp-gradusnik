#include <OneWire.h>

#define ONE_WIRE_BUS 5 // PD5

OneWire ds(ONE_WIRE_BUS);

/* ROM = 28 D4 38 F7 2 0 0 4    */
/* ROM = 28 FF 63 BE B1 16 3 20 */

byte addr[8] = { 0x28, 0xFF, 0x63, 0xBE, 0xB1, 0x16, 0x03, 0x20 };

byte data[12];

#define LED 6

#define A A4
#define B A2
#define C 10
#define D 8
#define E 7
#define J A3 // Это F
#define G 11

#define CA2 A0
#define CA1 13
#define CA3 A1

#define DP 9      // Точка

const int segs[7] = { A, B, C, D, E, J, G };

const byte numbers[10] = {
  0b1000000,
  0b1111001,
  0b0100100,
  0b0110000,
  0b0011001,
  0b0010010,
  0b0000010,
  0b1111000,
  0b0000000,
  0b0010000
};

const byte m[1] = {
  0b1000000
};

long previousMillis = 0;
long interval = 1000;
unsigned long currentMillis;

boolean run = false;

int thousands = 0;
int hundreds = 0;
int tens = 0;
int ones = 0;


void setup() {

  pinMode(LED, OUTPUT);

  pinMode(13, OUTPUT); // Digital 1
  pinMode(A0, OUTPUT); // Digital 2
  pinMode(A1, OUTPUT); // Digital 3

  pinMode(A4, OUTPUT); // SEG A
  pinMode(A2, OUTPUT); // SEG B
  pinMode(10, OUTPUT); // SEG C
  pinMode(8, OUTPUT); // SEG D
  pinMode(7, OUTPUT); // SEG E
  pinMode(A3, OUTPUT); // SEG F
  pinMode(11, OUTPUT); // SEG G

  pinMode(DP, OUTPUT); // DP

  digitalWrite(LED, HIGH);

  Serial.begin(9600);

}

int16_t raw;
int celsius;
int pcelsius;

byte i;
bool minus = false;

void loop() {

  currentMillis = millis();

  if (thousands > 0) {
    lightDigit1(numbers[thousands]); // temp%10]);
    delay(5);
  }

  if (minus) {
    dis_minus();
    delay(5);
  }

  lightDigit2(numbers[hundreds]); // int(temp/10)]);
  delay(5);
  lightDigit3(numbers[tens]); // int(8)]);
  delay(5);

  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;

    if (run) {
      ds.reset();
      ds.select(addr);
      ds.write(0xBE);
      for (i = 0; i < 9; i++) data[i] = ds.read();
      run = false;

      raw = (data[1] << 8) | data[0];
      
      // raw = 0xFFF8;  // -0.5
      // raw = 0xFE6F;  // -25.12

      celsius = ((float)raw / 16.0) * 100;

      // celsius = -99;
      
      pcelsius = celsius;
     
      if (celsius < 0) {
        celsius = celsius * -1;
        pcelsius = pcelsius * -1;     
        if (pcelsius > 999) celsius = celsius / 10;
        minus = true;
      } else {
        minus = false;
      }

      thousands = celsius / 1000;
      hundreds = (celsius % 1000) / 100;
      tens = (celsius % 100) / 10;
      ones = celsius % 10;
    }

    if (!run) {
      ds.reset();
      ds.select(addr);
      ds.write(0x44, 1);
      run = true;
    }

    if (digitalRead(LED) == 1) {
      digitalWrite(LED, LOW);
    } else {
      digitalWrite(LED, HIGH);
    }
  }

}

void dis_minus() {
  digitalWrite(CA1, HIGH);
  digitalWrite(CA2, LOW);
  digitalWrite(CA3, LOW);
  digitalWrite(DP, HIGH);
  digitalWrite(segs[6], 0);
  digitalWrite(segs[5], 1);
  digitalWrite(segs[4], 1);
  digitalWrite(segs[3], 1);
  digitalWrite(segs[2], 1);
  digitalWrite(segs[1], 1);
  digitalWrite(segs[0], 1);

}

void lightDigit1(byte number) {
  digitalWrite(CA1, HIGH);
  digitalWrite(CA2, LOW);
  digitalWrite(CA3, LOW);
  digitalWrite(DP, HIGH);
  lightSegments(number);
}

void lightDigit2(byte number) {
  digitalWrite(CA1, LOW);
  digitalWrite(CA2, HIGH);
  digitalWrite(CA3, LOW);
  if (!minus) {
    digitalWrite(DP, LOW);
  } else {
    if (pcelsius <= 999) {
      digitalWrite(DP, LOW);
    } else {
      digitalWrite(DP, HIGH);
    }
  }
  lightSegments(number);
}

void lightDigit3(byte number) {
  digitalWrite(CA1, LOW);
  digitalWrite(CA2, LOW);
  digitalWrite(CA3, HIGH);
  digitalWrite(DP, HIGH);
  lightSegments(number);
}

void lightSegments(byte number) {
  for (int i = 0; i < 7; i++) {
    int bit = bitRead(number, i);
    digitalWrite(segs[i], bit);
  }
}
