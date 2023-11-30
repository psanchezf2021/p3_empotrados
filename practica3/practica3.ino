#include <LiquidCrystal.h>
#include <Thread.h>
#include <DHT.h>
#include <StaticThreadController.h>
#include <ThreadController.h>
#include <avr/wdt.h>

#define DHTPIN 5
#define DHTTYPE DHT11

const int LED1 = A5;
const int LED2 = 3;
const int rs = 11, en = 10, d4 = 9, d5 = 8, d6 = 7, d7 = 6;
const int Trigger = 12;
const int Echo = 13;
const int pinJoyX = A0;
const int pinJoyY = A1;
const int pinJoyButton = 4;
const int INT_PIN = 2;

int coffee_position = 0;
int admin_position = 0;
String coffee_list[] = {"Cafe solo", "Cafe Cortado", "Cafe Doble", "Cafe Premium", "Chocolate"};
double prices_list[] = {1.00, 1.10, 1.25, 1.50, 2.00};
String admin_list[] = {"temperatura", "distancia sensor", "contador", "Modificar precios"};
int state = 0;
long randNumber;
long time_start = 0;
long t_ultra;
long d;
int Xvalue = 0;
int Yvalue = 0;
bool buttonValue = 1;
bool interrupt = false;
long interrupt_start = 0;
long interrupt_end = 0;
double price = 0;
float t = 0;
float h = 0;
int brightness = 0;

DHT dht (DHTPIN, DHTTYPE);

ThreadController controller = ThreadController();
Thread ultraS = Thread();
Thread joystick = Thread();
Thread temp = Thread();

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void stop() {
  noInterrupts();
  if(digitalRead(INT_PIN) == 0){
    interrupt_start = millis();
  } else{
    if(millis() - interrupt_start >= 2000 && millis() - interrupt_start <= 3000 && (state == 1 || state == 2 || state == 3)) {
      time_start = millis();
      state = 0;
    }else if(millis() - interrupt_start >= 5000) {
      if (state != 4) {
        digitalWrite(LED1, HIGH);
        digitalWrite(LED2, HIGH);
        state = 4;
      } else if (state == 4){
        digitalWrite(LED1, LOW);
        digitalWrite(LED2, LOW);
        time_start = millis();
        state = 0;
      }
    }
  }  
  interrupts();
}

void callback_ultraS() {
  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);          
  digitalWrite(Trigger, LOW);
  t_ultra = pulseIn(Echo, HIGH);
  d = t_ultra/59; 
}

void callback_joystick() {  
  Xvalue = analogRead(pinJoyX);
  Yvalue = analogRead(pinJoyY);
  buttonValue = digitalRead(pinJoyButton);
}

void callback_temp() {
  t = dht.readTemperature();
  h = dht.readHumidity();
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
  pinMode(INT_PIN, INPUT_PULLUP);

  ultraS.enabled = true;
  ultraS.setInterval(1000);
  ultraS.onRun(callback_ultraS);

  controller.add(&ultraS);

  joystick.enabled = true;
  joystick.setInterval(100);
  joystick.onRun(callback_joystick);

  controller.add(&joystick);

  temp.enabled = true;
  temp.setInterval(1000);
  temp.onRun(callback_temp);

  controller.add(&temp);

  dht.begin();

  attachInterrupt(digitalPinToInterrupt(INT_PIN), stop, CHANGE);

  randomSeed(analogRead(0));
  
  for (int i = 0; i <= 2; i++) {
    digitalWrite(LED1, HIGH);
    delay(1000);
    digitalWrite(LED1, LOW);
    delay(1000);
  }
  lcd.clear();

  wdt_disable();
  wdt_enable(WDTO_500MS);
}

void loop() {
  controller.run();

  switch (state){
    case 0:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ESPERANDO");
      lcd.setCursor(0, 1);
      lcd.print("CLIENTE");
      if(d < 100){
        time_start = millis();
        state = 1;
      }
      break;

    case 1:
      if(millis() - time_start < 5000){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Temp: " + String(t) + " C");
        lcd.setCursor(0, 1);
        lcd.print("Hum: " + String(h) + " %");
      } else {
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
  
        if(buttonValue == 0){
          randNumber = random(4000, 8000);
          time_start= millis();
          state = 2;
        }
      }
      break;
      
    case 2:
      if((millis() - time_start) >= randNumber){
        time_start= millis();
        state = 3;
        brightness = 0;
        analogWrite(LED2, brightness);
      } else {
        analogWrite(LED2, brightness);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Preparando Cafe ...");
        brightness += randNumber/2000;
      }
      break;

    case 3:
      if((millis() - time_start) >= 3000){
        time_start = millis();
        state = 0;
      } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("RETIRE BEBIDA");
      }
      break;
      
    case 4:
      lcd.clear();
      if(admin_position != 3){
        lcd.setCursor(0, 0);
        lcd.print("Ver");
      }
      lcd.setCursor(0, 1);
      lcd.print(admin_list[admin_position]);
    
      if(Yvalue > 950){
        admin_position--;
      } else if (Yvalue < 100){
        admin_position++;
      }

      if (admin_position > 3){
        admin_position = 0;
      } else if (admin_position < 0) {
        admin_position = 3;
      }

      if(buttonValue == 0){
        state = 5;
      }
      break;
      
    case 5:
      if(Xvalue < 100){
        state = 4;
      }
      
      switch (admin_position){
        case 0:
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Temp: " + String(t) + " C");
          lcd.setCursor(0, 1);
          lcd.print("Hum: " + String(h) + " %");
          
          break;
          
        case 1:
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Distancia: " + String(d) + " cm");
          break;
          
        case 2:
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(String(millis()/1000) + " s");
          break;
          
        case 3:
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

          if(buttonValue == 0){
            price = prices_list[coffee_position];
            admin_position = 4;
          }
          
          break;
          
        case 4:
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print(coffee_list[coffee_position]);
          lcd.setCursor(0, 1);
          lcd.print(price);

          if(Yvalue > 950){
            price -= 0.05;
          } else if (Yvalue < 100){
            price += 0.05;
          }

          if(buttonValue == 0){
            prices_list[coffee_position] = price;
            admin_position = 3;
          }
          
          break;
      }
      break;
  }

  delay(150);
  wdt_reset();   
}
