#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

RTC_DS3231 rtc;

const int chipSelect = 53;

int Button_1 = 2;
int Button_2 = 3;
int Button_3 = 4;
int Button_4 = 17;

#define ENC_A 18
#define ENC_B 19

unsigned long _lastIncReadTime = micros();
unsigned long _lastDecReadTime = micros();
int _pauseLength = 25000;
int _fastIncrement = 10;

volatile int counter = 0;
volatile int counter_1 = 0;
volatile int counter_2 = 0;
volatile int counter_3 = 0;

volatile int counter_4 = 0;
volatile int counter_5 = 0;
volatile int counter_6 = 0;

volatile int counter_7 = 0;
volatile int counter_8 = 0;
volatile int counter_9 = 0;

volatile int Total_1 = 0;
volatile int Total_2 = 0;
volatile int Total_3 = 0;

volatile int Total_a = 0;
volatile int Total_b = 0;
volatile int Total_c = 0;

volatile int Total_d = 0;
volatile int Total_e = 0;
volatile int Total_f = 0;

volatile int Total_g = 0;
volatile int Total_h = 0;
volatile int Total_i = 0;

int currentLine = 0;
bool cursorBlink = true;
static int lastCounter = 0;

volatile boolean buttonState1 = HIGH;
volatile boolean lastButtonState1 = HIGH;
volatile unsigned long lastDebounceTime1 = 0;
volatile unsigned long debounceDelay1 = 50;

volatile boolean buttonState2 = HIGH;
volatile boolean lastButtonState2 = HIGH;
volatile unsigned long lastDebounceTime2 = 0;
volatile unsigned long debounceDelay2 = 50;

volatile boolean buttonState3 = HIGH;
volatile boolean lastButtonState3 = HIGH;
volatile unsigned long lastDebounceTime3 = 0;
volatile unsigned long debounceDelay3 = 50;

volatile boolean buttonState4 = HIGH;
volatile boolean lastButtonState4 = HIGH;
volatile unsigned long lastDebounceTime4 = 0;
volatile unsigned long debounceDelay4 = 50;

enum State {
  IDLE,
  SERVICE_1,
  SERVICE_2,
  SERVICE_3,
  SERVICE_1_COUNTER,
  SERVICE_2_COUNTER,
  SERVICE_3_COUNTER
};

State currentState = IDLE;

LiquidCrystal_I2C lcd(0x3F, 20, 4);

void setup() {
  Serial.begin(9600);

  pinMode(Button_1, INPUT_PULLUP);
  pinMode(Button_2, INPUT_PULLUP);
  pinMode(Button_3, INPUT_PULLUP);
  pinMode(Button_4, INPUT_PULLUP);
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENC_A), read_encoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B), read_encoder, CHANGE);

  attachInterrupt(digitalPinToInterrupt(Button_1), buttonInterrupt1_Service_1, FALLING);
  attachInterrupt(digitalPinToInterrupt(Button_2), buttonInterrupt2_Service_1, FALLING);
  attachInterrupt(digitalPinToInterrupt(Button_3), buttonInterrupt3_Service_1, FALLING);

  attachInterrupt(digitalPinToInterrupt(Button_1), buttonInterrupt4_Service_2, FALLING);
  attachInterrupt(digitalPinToInterrupt(Button_2), buttonInterrupt5_Service_2, FALLING);
  attachInterrupt(digitalPinToInterrupt(Button_3), buttonInterrupt6_Service_2, FALLING);
  
  attachInterrupt(digitalPinToInterrupt(Button_1), buttonInterrupt7_Service_3, FALLING);
  attachInterrupt(digitalPinToInterrupt(Button_2), buttonInterrupt8_Service_3, FALLING);
  attachInterrupt(digitalPinToInterrupt(Button_3), buttonInterrupt9_Service_3, FALLING);

  attachInterrupt(digitalPinToInterrupt(Button_4), buttonInterrupt4_Service1, FALLING);
  attachInterrupt(digitalPinToInterrupt(Button_4), buttonInterrupt4_Service2, FALLING);
  attachInterrupt(digitalPinToInterrupt(Button_4), buttonInterrupt4_Service3, FALLING);

  // Check if the RTC is connected
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1)
      ;
  }
  // Initialize SD card
  if (!SD.begin(chipSelect)) {
    Serial.println("SD Card initialization failed!");
    while (1)
      ;
  }

  // Check if the RTC lost power and if so, set the time
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(7, 1);
  lcd.print("SYSTEM");
  lcd.setCursor(3, 2);
  lcd.print("INITIALIZATION");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SELECT SERVICE BELOW");
  lcd.setCursor(0, 1);
  lcd.print("1. SERVICE 1");
  lcd.setCursor(0, 2);
  lcd.print("2. SERVICE 2");
  lcd.setCursor(0, 3);
  lcd.print("3. SERVICE 3");
  delay(1000);
}

void loop() {
  switch (currentState) {
    case IDLE:
      idle();
      break;
    case SERVICE_1_COUNTER:
      service_1_counter();
      break;
    case SERVICE_2_COUNTER:
      service_2_counter();
      break;
    case SERVICE_3_COUNTER:
      service_3_counter();
      break;
  }
}
void idle() {
  if (counter != lastCounter) {
    Serial.println(counter);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SELECT SERVICE BELOW");
    lcd.setCursor(0, 1);
    lcd.print("1. SERVICE 1");
    lcd.setCursor(0, 2);
    lcd.print("2. SERVICE 2");
    lcd.setCursor(0, 3);
    lcd.print("3. SERVICE 3");
    // Set the cursor and blink state based on the current service
    if (counter == 1) {
      lcd.setCursor(13, 1);
      lcd.blink();
      lastCounter = counter;
    } else if (counter == 2) {
      lcd.setCursor(13, 2);
      lcd.blink();
      lastCounter = counter;
    } else if (counter == 3) {
      lcd.setCursor(13, 3);
      lcd.blink();
      lastCounter = counter;
    }
  }
  if (counter == 1) {
    buttonInterrupt4_Service1();
  } else if (counter == 2) {
    buttonInterrupt4_Service2();
  } else if (counter == 3) {
    buttonInterrupt4_Service3();
  }
  delay(500);
}
void service_1_counter() {
  buttonInterrupt1_Service_1();
  buttonInterrupt2_Service_1();
  buttonInterrupt3_Service_1();

  lcd.clear();
  lcd.noBlink();
  lcd.setCursor(0, 0);
  lcd.print("SERVICE 1");

  DateTime now = rtc.now();
  lcd.setCursor(11, 0);
  lcd.print(now.day(), DEC);
  lcd.print('/');
  lcd.print(now.month(), DEC);
  lcd.print('/');
  lcd.print(now.year(), DEC);

  lcd.setCursor(0, 1);
  lcd.print("MEN: ");
  Total_a = counter_1;
  lcd.print(counter_1);
  lcd.setCursor(0, 2);
  lcd.print("WOMEN: ");
  Total_b = counter_2;
  lcd.print(counter_2);
  lcd.setCursor(0, 3);
  lcd.print("YOUTH: ");
  Total_c = counter_3;
  lcd.print(counter_3);
  lcd.setCursor(10, 3);
  lcd.print("TOTAL: ");
  Total_1 = Total_a + Total_b + Total_c;
  lcd.print(Total_1);
  buttonInterrupt4_Service1_Back();
  delay(500);
}
void service_2_counter() {
  buttonInterrupt4_Service_2();
  buttonInterrupt5_Service_2();
  buttonInterrupt6_Service_2();
  lcd.clear();
  lcd.noBlink();
  lcd.setCursor(0, 0);
  lcd.print("SERVICE 2");

  DateTime now = rtc.now();
  lcd.setCursor(11, 0);
  lcd.print(now.day(), DEC);
  lcd.print('/');
  lcd.print(now.month(), DEC);
  lcd.print('/');
  lcd.print(now.year(), DEC);

  lcd.setCursor(0, 1);
  lcd.print("MEN: ");
  Total_d = counter_4;
  lcd.print(counter_4);
  lcd.setCursor(0, 2);
  lcd.print("WOMEN: ");
  Total_e = counter_5;
  lcd.print(counter_5);
  lcd.setCursor(0, 3);
  lcd.print("YOUTH: ");
  Total_f = counter_6;
  lcd.print(counter_6);
  lcd.setCursor(10, 3);
  Total_2 = Total_d + Total_e + Total_f;
  lcd.print("TOTAL: ");
  lcd.print(Total_2);
  buttonInterrupt4_Service1_Back();
  delay(500);
}
void service_3_counter() {
  buttonInterrupt7_Service_3();
  buttonInterrupt8_Service_3();
  buttonInterrupt9_Service_3();
  lcd.clear();
  lcd.noBlink();
  lcd.setCursor(0, 0);
  lcd.print("SERVICE 3");

  DateTime now = rtc.now();
  lcd.setCursor(11, 0);
  lcd.print(now.day(), DEC);
  lcd.print('/');
  lcd.print(now.month(), DEC);
  lcd.print('/');
  lcd.print(now.year(), DEC);
  lcd.setCursor(0, 1);
  lcd.print("MEN: ");
  Total_g = counter_7;
  lcd.print(counter_7);
  lcd.setCursor(0, 2);
  lcd.print("WOMEN: ");
  Total_h = counter_8;
  lcd.print(counter_8);
  lcd.setCursor(0, 3);
  lcd.print("YOUTH: ");
  Total_i = counter_9;
  lcd.print(counter_9);
  lcd.setCursor(10, 3);
  Total_3 = Total_g + Total_h + Total_i;
  lcd.print("TOTAL: ");
  lcd.print(Total_3);
  buttonInterrupt4_Service1_Back();
  delay(500);
}
void updateSDCardData() {
  // Open the file in read mode
  File dataFile = SD.open("data.csv", FILE_READ);

  // If the file is not available, return
  if (!dataFile) {
    Serial.println("Error opening data file");
    return;
  }

  // Read the existing data
  String newData = "";
  while (dataFile.available()) {
    newData += (char)dataFile.read();
  }

  // Close the file
  dataFile.close();

  // Find the position of the last entry for Service_1 and today's date
  String searchString = getCurrentDate() + ",Service_1";
  int pos = newData.lastIndexOf(searchString);

  if (pos != -1) {
    // If the entry is found, update the values
    newData.remove(pos, newData.length() - pos);  // Remove the existing entry

    // Update the time, Men, Women, Youth, and Total values
    newData += getCurrentDate() + "," + getCurrentTime() + ",Service_1," + counter_1 + "," + counter_2 + "," + counter_3 + "," + Total_1 + "\n";
  } else {
    // If no entry is found, add a new entry
    newData += getCurrentDate() + "," + getCurrentTime() + ",Service_1," + counter_1 + "," + counter_2 + "," + counter_3 + "," + Total_1 + "\n";
  }

  // Open the file in write mode to update the data
  dataFile = SD.open("data.csv", FILE_WRITE);
  Serial.println("Test Pass");

  // If the file is not available, return
  if (!dataFile) {
    Serial.println("Error opening data file for writing");
    return;
  }

  // Write the updated data to the file
  dataFile.print(newData);
  Serial.println("Data Logged");

  // Close the file
  dataFile.close();
}

String getCurrentTime() {
  DateTime now = rtc.now();
  String timeStr = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
  return timeStr;
}

String getCurrentDate() {
  DateTime now = rtc.now();
  String dateStr = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year());
  return dateStr;
  Serial.println(dateStr);
}
void buttonInterrupt1_Service_1() {
  if (millis() - lastDebounceTime1 > debounceDelay1) {
    buttonState1 = digitalRead(Button_1);
    if (buttonState1 == LOW && lastButtonState1 == HIGH) {
      Serial.println("Button 1 Pressed!");
      counter_1++;
    }
    lastButtonState1 = buttonState1;
    lastDebounceTime1 = millis();
  }
}
void buttonInterrupt2_Service_1() {
  if (millis() - lastDebounceTime2 > debounceDelay2) {
    buttonState2 = digitalRead(Button_2);
    if (buttonState2 == LOW && lastButtonState2 == HIGH) {
      Serial.println("Button 2 Pressed!");
      counter_2++;
    }
    lastButtonState2 = buttonState2;
    lastDebounceTime2 = millis();
  }
}
void buttonInterrupt3_Service_1() {
  if (millis() - lastDebounceTime3 > debounceDelay3) {
    buttonState3 = digitalRead(Button_3);
    if (buttonState3 == LOW && lastButtonState3 == HIGH) {
      Serial.println("Button 3 Pressed!");
      counter_3++;
    }
    lastButtonState3 = buttonState3;
    lastDebounceTime3 = millis();
  }
}
void buttonInterrupt4_Service_2() {
  if (millis() - lastDebounceTime1 > debounceDelay1) {
    buttonState1 = digitalRead(Button_1);
    if (buttonState1 == LOW && lastButtonState1 == HIGH) {
      Serial.println("Button 1 Pressed!");
      counter_4++;
    }
    lastButtonState1 = buttonState1;
    lastDebounceTime1 = millis();
  }
}
void buttonInterrupt5_Service_2() {
  if (millis() - lastDebounceTime2 > debounceDelay2) {
    buttonState2 = digitalRead(Button_2);
    if (buttonState2 == LOW && lastButtonState2 == HIGH) {
      Serial.println("Button 2 Pressed!");
      counter_5++;
    }
    lastButtonState2 = buttonState2;
    lastDebounceTime2 = millis();
  }
}
void buttonInterrupt6_Service_2() {
  if (millis() - lastDebounceTime3 > debounceDelay3) {
    buttonState3 = digitalRead(Button_3);
    if (buttonState3 == LOW && lastButtonState3 == HIGH) {
      Serial.println("Button 3 Pressed!");
      counter_6++;
    }
    lastButtonState3 = buttonState3;
    lastDebounceTime3 = millis();
  }
}
void buttonInterrupt7_Service_3() {
  if (millis() - lastDebounceTime3 > debounceDelay3) {
    buttonState3 = digitalRead(Button_1);
    if (buttonState3 == LOW && lastButtonState3 == HIGH) {
      Serial.println("Button 1 Pressed!");
      counter_7++;
    }
    lastButtonState3 = buttonState3;
    lastDebounceTime3 = millis();
  }
}

void buttonInterrupt8_Service_3() {
  if (millis() - lastDebounceTime3 > debounceDelay3) {
    buttonState3 = digitalRead(Button_2);
    if (buttonState3 == LOW && lastButtonState3 == HIGH) {
      Serial.println("Button 2 Pressed!");
      counter_8++;
    }
    lastButtonState3 = buttonState3;
    lastDebounceTime3 = millis();
  }
}
void buttonInterrupt9_Service_3() {
  if (millis() - lastDebounceTime3 > debounceDelay3) {
    buttonState3 = digitalRead(Button_3);
    if (buttonState3 == LOW && lastButtonState3 == HIGH) {
      Serial.println("Button 3 Pressed!");
      counter_9++;
    }
    lastButtonState3 = buttonState3;
    lastDebounceTime3 = millis();
  }
}
void buttonInterrupt4_Service1() {
  if (millis() - lastDebounceTime4 > debounceDelay4) {
    buttonState4 = digitalRead(Button_4);
    if (buttonState4 == LOW && lastButtonState4 == HIGH) {
      Serial.println("Service 1 Selected");
      if (currentState == IDLE) {
        currentState = SERVICE_1_COUNTER;
      }
    }
    lastButtonState4 = buttonState4;
    lastDebounceTime4 = millis();
  }
}
void buttonInterrupt4_Service1_Back() {
  if (millis() - lastDebounceTime4 > debounceDelay4) {
    buttonState4 = digitalRead(Button_4);
    if (buttonState4 == LOW && lastButtonState4 == HIGH) {
      Serial.println("Select Options");
      if (currentState == SERVICE_1_COUNTER || currentState == SERVICE_2_COUNTER || currentState == SERVICE_3_COUNTER ) {
        currentState = IDLE;
      }
    }
    lastButtonState4 = buttonState4;
    lastDebounceTime4 = millis();
  }
}
void buttonInterrupt4_Service2() {
  if (millis() - lastDebounceTime4 > debounceDelay4) {
    buttonState4 = digitalRead(Button_4);
    if (buttonState4 == LOW && lastButtonState4 == HIGH) {
      Serial.println("Service 1 Selected");
      currentState = SERVICE_2_COUNTER;
    }
    lastButtonState4 = buttonState4;
    lastDebounceTime4 = millis();
  }
}
void buttonInterrupt4_Service3() {
  if (millis() - lastDebounceTime4 > debounceDelay4) {
    buttonState4 = digitalRead(Button_4);
    if (buttonState4 == LOW && lastButtonState4 == HIGH) {
      Serial.println("Service 1 Selected");
      currentState = SERVICE_3_COUNTER;
    }
    lastButtonState4 = buttonState4;
    lastDebounceTime4 = millis();
  }
}
void read_encoder() {
  static uint8_t old_AB = 3;
  static int8_t encval = 0;
  static const int8_t enc_states[] = { 0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0 };

  old_AB <<= 2;

  if (digitalRead(ENC_A)) old_AB |= 0x02;
  if (digitalRead(ENC_B)) old_AB |= 0x01;

  encval += enc_states[(old_AB & 0x0f)];

  if (encval > 3) {  // Four steps forward
    int changevalue = 1;
    if ((micros() - _lastIncReadTime) < _pauseLength) {
      changevalue = _fastIncrement * changevalue;
    }
    _lastIncReadTime = micros();
    counter = (counter % 3) + 1;  // Wrap-around when exceeding 3
    encval = 0;
  } else if (encval < -3) {  // Four steps backward
    int changevalue = -1;
    if ((micros() - _lastDecReadTime) < _pauseLength) {
      changevalue = _fastIncrement * changevalue;
    }
    _lastDecReadTime = micros();
    counter = ((counter - 2 + 3) % 3) + 1;  // Wrap-around when below 1
    encval = 0;
  }
}
