#include <Keypad.h>
#include <Servo.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {8,7,6,5};
byte colPins[COLS] = {4,3,2}; 
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

int num[4] = {0,0,0,0};
int pass[4];

Servo myservo;
int pos = 90;
bool locked = true;

LiquidCrystal lcd(A0,A1,A2,A3,A4,A5);
 
void setup() {
  Serial.begin(9600);
  myservo.attach(9); 
  lcd.begin(16, 2);
  for(int i = 0 ; i < 4 ; i++){
    pass[i] = EEPROM.read(i);
  } 
  menu();
}

int x = 0;
void loop(){
  char key = kpd.getKey();
  if(key){
    switch (key) {
      case '1':
      setPassword();
      menu();
      break;
    case '2':
      enterPassword();
      menu();
      break;
    case '*':
      x++;
      if(x==4){
        showPassword();
        x = 0;
      }
      menu();
      break;
    case '3':
      lock('b');
      menu();
      break;
    default:
      Serial.print("wrong choice ");
      Serial.println(key);
      menu();
    }
  }   
}

void menu(){
  Serial.println("Please select a command.");
  Serial.println("1 - set password");
  Serial.println("2 - enter password");
  Serial.println("* - show password");
  Serial.println("3 - lock");
  lcd.clear();
  lcd.print("1:Change 2:Enter");
  lcd.setCursor(0, 1);
  lcd.print("3:Lock");
}

void setPassword(){
  check();
  Serial.println("\nEnter new password: ");
  lcd.clear();
  lcd.print("New Password");
  lcd.setCursor(0, 1);
  lcd.print(":");
  input();
  Serial.println("");
  for(int i = 0 ; i < 4 ; i++) {
    EEPROM.write(i,num[i]);
    pass[i] = num[i];
  }
}

void check(){
  Serial.println("Enter your current password:");
  lcd.clear();
  lcd.print("Current Password");
  lcd.setCursor(0, 1);
  lcd.print(":");
  input();
  if(!isCorrect()){
    Serial.println("\nincorrect");
    lcd.clear();
    lcd.print("incorrect");
    lcdScreenBlink();
    check();
  }
}

void enterPassword(){
  Serial.print("Password: ");
  lcd.clear();
  lcd.print("Password");
  lcd.setCursor(0, 1);
  lcd.print(":");
  input();
  if(!isCorrect()){
    Serial.println("\nIncorrect password, please enter again");
    lcd.clear();
    lcd.print("incorrect");
    lcdScreenBlink();
    enterPassword();
  }
  else{
    Serial.println("\ncorrect");
    lcd.clear();
    lcd.print("correct");
    lock('a');
    locked = false;        
  }
}

void showPassword() {
  lcd.clear();
  Serial.print("Password: ");
  for(int i = 0 ; i < 4 ; i++){
    int val = EEPROM.read(i);
    Serial.write(val);
    lcd.print(val-'0');
  }
  Serial.println("");
  while(true){
    char key = kpd.getKey();
    if(key)
      break;
  }
}

void lock(char choice){
  if(choice == 'b'){
    if(!locked){
      for (pos = 0; pos <= 90; pos += 1) {
        myservo.write(pos);
        delay(15);
      }

      locked = true;
      Serial.println("locking");
    }
  }
  else if (choice == 'a'){
      for (pos = 90; pos >= 0; pos -= 1) {
        myservo.write(pos);
        delay(15);
      }
  }
}

void input(){
    for(int i = 0; i < 4;){
    char key = kpd.getKey();
    if(key) {
      switch (key) {
        case '*':
          break;
        case '#':
          break;
        default:
          Serial.print(key);
          lcd.print("*");
          num[i] = key;
          i++;
      }
    }
    else{
      lcd.blink();
    }
  }
  lcd.noBlink();
}

void lcdScreenBlink(){
    delay(500);
    lcd.noDisplay();
    delay(200);
    lcd.display();
    delay(200);
    lcd.noDisplay();
    delay(200);
    lcd.display();
    delay(500);
}

boolean isCorrect(){
  for(int i = 0 ; i < 4 ; i++){
    if(num[i] != pass[i]) return false;
  }
  return true;
}

