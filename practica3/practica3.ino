#include <LiquidCrystal.h>
#include <Thread.h>

int LED1 = 5;
const int rs = 11, en = 10, d4 = 9, d5 = 8, d6 = 7, d7 = 6;
const int Trigger = 12;
const int Echo = 13;
const int pinJoyX = A0;
const int pinJoyY = A1;
const int pinJoyButton = 3;

int coffee_position = 0;
String coffee_list[] = {"Cafe solo", "Cafe Cortado", "Cafe Doble", "Cafe Premium", "Chocolate"};
String prices_list[] = {"1.00", "1.10", "1.25", "1.50", "2.00"};
int state = 0;
long randNumber;
long time_start = 0;
long t;
long d;

Thread ultraS = Thread();
Thread joystick = Thread();
Thread temp = Thread();

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void callback_ultraS() {
  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);          
  digitalWrite(Trigger, LOW);
  t = pulseIn(Echo, HIGH);
  d = t/59; 
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("CARGANDO...");
  pinMode(LED1, OUTPUT);
  pinMode(Trigger, OUTPUT);
  pinMode(Echo, INPUT);
  pinMode(pinJoyButton , INPUT_PULLUP);

  ultraS.enabled = true;
  ultraS.setInterval(1000);
  ultraS.onRun(callback_ultraS);

  //joystick.enabled = true;
  //joystick.setInterval(100);
  //joystick.onRun(callback_joystick);

  randomSeed(analogRead(0));
  for (int i = 0; i <= 2; i++) {
    digitalWrite(LED1, HIGH);
    delay(1000);
    digitalWrite(LED1, LOW);
    delay(1000);
  }
  lcd.clear();
}

void loop() {
  int Xvalue = 0;
  int Yvalue = 0;
  bool buttonValue = 1;

  ultraS.run();

  Xvalue = analogRead(pinJoyX);
  Yvalue = analogRead(pinJoyY);
  buttonValue = digitalRead(pinJoyButton);

  switch (state){
    case 0:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ESPERANDO");
      lcd.setCursor(0, 1);
      lcd.print("CLIENTE");
      if(d < 100){
        state = 1;
      }
      break;

    case 1:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(coffee_list[coffee_position]);
      lcd.setCursor(0, 1);
      lcd.print(prices_list[coffee_position]);
    
      if(Yvalue > 950){
        coffee_position--;
      } else if (Yvalue < 100){
        coffee_position++;
      }
    
      if (coffee_position > 4){
        coffee_position = 0;
      } else if (coffee_position < 0) {
        coffee_position = 4;
      }

      if(d > 100){
        state = 0;
      }

      if(buttonValue == 0){
        randNumber = random(4000, 8000);
        time_start= millis();
        state = 2;
      }
      break;
      
    case 2:
      if((millis() - time_start) >= randNumber){
        time_start= millis();
        state = 3;
      } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Preparando Cafe ...");
      }
      break;

    case 3:
      if((millis() - time_start) >= 3000){
        state = 1;
      } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("RETIRE BEBIDA");
      }
      break;
  }

  delay(200);   
}
