/*This for my Embedded course project. But I try to bring this to my 
real world usage for my parents especially for my mother.
Some additional features can be updated to this project later.
I acccess those through my GitHub https://github.com/Charlie174/Smart-MediBox*/

//Libriaries 
#include <Wire.h>
#include <Adafruit_GFX.h>         //for display
#include <Adafruit_SSD1306.h>     //for display
#include <WiFi.h>                 //connect to wifi and for time 
#include <time.h>                 //connect to wifi and for time 
#include <DHTesp.h>               //for humidity and tempreture 

//define OLDED // I2C protocols.
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

//define for alarm
#define buzzer 12              // 100 resistor is good
#define LED_R 18
#define LED_G 19
#define LED_B 5             //minimum 220 resistor

//dht pin definitions
#define DHTdata 16

//Buttons definitions   // 1k resistor needed
#define cancel_PB 35
#define up_PB 33
#define down_PB 32
#define ok_PB 34

//for setting time zone (Universal time)
int O_S_minutes = 0;
int O_S_hours = 0;

//for WiFI time updation
// #define UTC_OFFSET     O_S_hours * 60 * 60 + O_S_minutes * 60
unsigned long UTC_OFFSET = 0;
#define UTC_OFFSET_DST 0
#define NTP_SERVER     "pool.ntp.org"



//Set the objects for OLED configuration
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//seting up DHT sensor configuration
DHTesp dhtSensor;


//Global variables for time 
int days = 0;
int hours = 0;
int minutes = 0;
int seconds = 0;

//Manually update the time.
//unsigned long timeCurrent = 0;
//unsigned long timePrevious = 0;

//for ring the alarm through buzzer and stop via A.high push button with glowing LED
int num_of_chords = 7;
int C = 262;
int D = 294;
int E = 330;
int F = 349;
int G = 392;
int A = 440;
int B = 494;
int C_H = 523;
int chords[] = {C,D,E,F,G,A,B,C_H};

//for alarm sets
bool alarm_enabled = true;
int num_alarm = 3;
int alarm_hours[] = {23,23,20};
int alarm_minutes[] = {10,8,10};
bool alarm_triggered[] = {false, false, false};

//for selecting modes and options when we pressed the buttons
int current_mode = 0;
int max_modes = 5;
String options[] = {"1 :- Set Time Zone", "2 :- Set Alarm 1", "3 :- Set Alarm 2", "4 :- Set Alarm 3", "5 :- Remove Alarm"};


//........................................................................................................
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(buzzer,OUTPUT);
  pinMode(cancel_PB,INPUT);
  pinMode(up_PB,INPUT);
  pinMode(down_PB,INPUT);
  pinMode(ok_PB,INPUT);
  pinMode(LED_R,OUTPUT);
  pinMode(LED_B,OUTPUT);
  pinMode(LED_G,OUTPUT);

  //Check the display inicialization 
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)){
    Serial.println(F("OLED is not allocated"));
    for(;;);
  }

  display.display();
  delay(500);
  
  //DHT initialization 
  dhtSensor.setup(DHTdata, DHTesp::DHT22);

  //check and wait until it connected to WiFi
  WiFi.begin("Wokwi-GUEST", "", 6);        //Greet my mother 
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    display.clearDisplay();
    print_line("Connecting to WiFi ", 0, 0, 2);
  }
  //to tell that WiFi is connected 
  display.clearDisplay();
  print_line("Connected to WiFi ", 0, 0, 2);
  delay(100);

  //configure the time zone through WiFi NTP
  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);

  //Welcome Message, show only for onetime
  display.clearDisplay();
  print_line("welcome to Charlie's MediBox!", 0, 0, 2);
  delay(200);
  display.clearDisplay();
}


//.....................................................................................
void loop() {
  // put your main code here, to run repeatedly:

  //Show the time until user make any input 
  update_time_with_check_alarm();
  delay(300);

  //allow user to use go to menu using menu button
  if (digitalRead(ok_PB) == LOW){
    delay(1000);
    //Serial.println("Menu");
    go_to_menu();
  }

  //always check the humidity and temperature 
  check_TempAndHumidity();
  
  
  // Serial.println(digitalRead(ok_PB));
  // Serial.println(digitalRead(up_PB));
  // Serial.println(digitalRead(down_PB));
  // Serial.println(digitalRead(cancel_PB));
  // Serial.println(" ");
}


//FUNCTIONS I created...........................................................................

//Print any thing in the display with prefered size and position *******************************8
void print_line(String text, int row, int column, int text_size){
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(text_size);    
  display.setCursor(column, row);
  display.println(text);
  display.display();
}
//***********************************************************************************************


//************************************************************************************************
//to print the value always in 2 digit
String formatWithLeadingZeros(int value) {
    if (value < 10) {
        return "0" + String(value);
    } else {
        return String(value);
    }
}
//******************************************************************************************************


//******************************************************************************************************************
//print the time on display 
void print_time_now() {
    print_line(formatWithLeadingZeros(days), 0, 0, 2);
    print_line(":", 0, 20, 2);
    print_line(formatWithLeadingZeros(hours), 0, 30, 2);
    print_line(":", 0, 50, 2);
    print_line(formatWithLeadingZeros(minutes), 0, 60, 2);
    print_line(":", 0, 80, 2);
    print_line(formatWithLeadingZeros(seconds), 0, 90, 2);
}
//***********************************************************************************************************************


//***********************************************************************************************
//Manually update the time using millis()
// void update_time(){
//   timeCurrent = millis()/1000;
//   seconds = timeCurrent - timePrevious;

//   //one minute passed
//   if (seconds == 60){
//     minutes += 1;
//     timePrevious == 60; //millis continuously calculate time
//   }
  
//   //an hour passed
//   if (minutes == 60){
//     hours += 1;
//     minutes == 0;
//   }

//   //a day passed
//   if (hours == 24){
//     days += 1;
//     hours == 0;
//   }
// }
//***********************************************************************************************



//***********************************************************************************************
void update_time(){
  struct tm timeinfo;
  getLocalTime(&timeinfo);

  char day_str[8];
  char hour_str[8];
  char min_str[8];
  char sec_str[8];

  //time to string data type
  strftime(day_str, 8, "%d", &timeinfo);
  strftime(hour_str, 8, "%H", &timeinfo);
  strftime(min_str, 8, "%M", &timeinfo);
  strftime(sec_str, 8, "%S", &timeinfo);
  
  //save to our variables connected in other functions
  days = atoi(day_str);
  hours = atoi(hour_str);
  minutes = atoi(min_str);
  seconds = atoi(sec_str);
}
//***********************************************************************************************

//***********************************************************************************************
//ringing an alaram
void ring_alarm(){

  digitalWrite(LED_G,HIGH);

  bool break_happened = false; //to break from whole loop

  //respond to user input as stop the alarm 
  while(break_happened==false && digitalRead(cancel_PB)==HIGH){
    for (int i=0; i<8; i++){
      if (digitalRead(cancel_PB)==LOW){
        digitalWrite(LED_R,LOW);
        break_happened=true;
        break;
      }
      tone(buzzer, chords[i]);
      //Serial.println(chords[i]);
      delay(500);
      noTone(buzzer);
      delay(5);
    }
  }
}
//***********************************************************************************************


//***********************************************************************************************
//Update time with the function I create
//and check if the alarm are enabled.
void update_time_with_check_alarm(){
  display.clearDisplay();
  update_time(); //function for update time
  print_time_now(); //print the updated time

  //checking for the alarm 
  if (alarm_enabled){
    //check for all the alarms
    for (int i=0; i<num_alarm; i++){
      if (alarm_triggered[i]==false && hours == alarm_hours[i] && minutes==alarm_minutes[i]){
        ring_alarm();
        alarm_triggered[i]=true; // to stop the alarm in any stage if the alarm start to ring or at the moment
      }
    }
  }
}
//***********************************************************************************************


//***********************************************************************************************
//check the humidity and temperature with the threshold value and make alarm if not in range
void check_TempAndHumidity(){
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  bool all_good = true;

  //tempreture warnings 
  if (data.temperature > 32){
    all_good = false;
    //digitalWrite(LED_R,HIGH);
    print_line("TEMP HIGH", 30, 0, 2);
    play_danger();
  }
  else if (data.temperature < 26){
    all_good = false;
    //digitalWrite(LED_R,HIGH);
    print_line("TEMP LOW", 30, 0, 2);
    play_danger();
  }

  //Humidity warnings
  if (data.humidity > 80){
    all_good = false;
    //digitalWrite(LED_R,HIGH);
    print_line("HUM HIGH", 50, 0, 2);
    play_danger();
  }
  else if (data.humidity < 60){
    all_good = false;
    //digitalWrite(LED_R,HIGH);
    print_line("HUM LOW", 50, 0, 2);
    play_danger();
  }

  if (all_good){
    digitalWrite(LED_G,LOW);
  }

}
//***********************************************************************************************



//***********************************************************************************************
//user to interact with my medibox. choose what they want 
void go_to_menu(){
  current_mode = 0;        //always start from initial mode compactibled with user
  while (digitalRead(cancel_PB) == HIGH){
    display.clearDisplay();
    print_line(options[current_mode], 0, 0, 2);

    int pressed = wait_for_button_press();

    if (pressed == up_PB){
      current_mode += 1;
      current_mode %= max_modes;
      delay(200);
    }

    else if (pressed == down_PB){
      current_mode -= 1;
      if (current_mode < 0){
        current_mode = max_modes - 1;
      }
      delay(200);
    }
    
    else if (pressed == ok_PB){
      //Serial.println(current_mode);
      delay(200);
      run_mode(current_mode);
    }
  }
}
//****************************************************************************************************************************************


//*****************************************************************************************************************************************
void run_mode(int mode){
  if (mode == 0){
    set_time_zone();
  }

  else if (mode == 1 || mode == 2 || mode == 3){
    set_alarm(mode -1);
  }

  else if (mode == 4){
    alarm_enabled = false;
  }
}
//**********************************************************************************************************************************


//**********************************************************************************************************************************
int wait_for_button_press(){
  while (true){
    if (digitalRead(up_PB) == LOW){
      delay(200);
      return up_PB;
    }

    if (digitalRead(down_PB) == LOW){
      delay(200);
      return down_PB;
    }

    if (digitalRead(ok_PB) == LOW){
      delay(200);
      return ok_PB;
    }

    if (digitalRead(cancel_PB) == LOW){
      delay(200);
      return cancel_PB;
    }

    update_time();
  }
}
//***************************************************************************************************************************



//***************************************************************************************************************************
// void set_time(){
//   int temp_hour = hours;
//   while (true){
//     display.clearDisplay();
//     print_line("Enter hour: " + String(temp_hour), 0, 0, 2);

//     int pressed = wait_for_button_press();
//     if (pressed == up_PB){
//       delay(200);
//       temp_hour += 1;
//       temp_hour = temp_hour % 24;   //after 25 count from 1
//     }

//     else if (pressed == down_PB){
//       delay(200);
//       temp_hour -= 1;
//       if (temp_hour < 0){
//         temp_hour = 23;    // 0 said the midnight 12 am
//       }
//     }
    
//     else if (pressed == ok_PB){
//       delay(200);
//       hours = temp_hour;
//       break;
//     }

//     else if (pressed == cancel_PB){
//       delay(200);
//       break;
//     }
//   }

//   int temp_minute = minutes;
//   while(true){
//     display.clearDisplay();
//     print_line("Enter minute: " + String(temp_minute), 0, 0, 2);

//     int pressed = wait_for_button_press();
//     if (pressed == up_PB){
//       delay(200);
//       temp_minute += 1;
//       temp_minute = temp_minute % 60;    //after 60 count from 1
//     }

//     else if (pressed == down_PB){
//       delay(200);
//       temp_minute -= 1;
//       if (temp_minute < 0){
//         temp_minute = 59;    // 0 said that was 60th minute 
//       }
//     }
    
//     else if (pressed == ok_PB){
//       delay(200);
//       minutes = temp_minute;
//       break;
//     }

//     else if (pressed == cancel_PB){
//       delay(200);
//       break;
//     }    
//   }  

//   display.clearDisplay();
//   print_line("Time is set", 0, 0, 2);
//   delay(1000);
// }
//**************************************************************************************************************************



//*********************************************************************************************************************************
void set_time_zone() {
  //Get hours of utc offset as input
  int temp_hours = O_S_hours;
  while (true) {
    display.clearDisplay();
    print_line("Enter hours: " + String(temp_hours), 0, 0, 2);

    int button = wait_for_button_press();
    if (button == ok_PB) {
      delay(200);
      O_S_hours = temp_hours;
      break;
    }

    if (button == cancel_PB) {
      delay(200);
      break;
    }

    if (button == up_PB) {
      delay(200);
      temp_hours += 1;
      temp_hours %= 24;
    }

    else if (button == down_PB) {
      delay(200);
      temp_hours -= 1;
      if (temp_hours < 0) {
        temp_hours += 24;
      }
    }
  }

  //Get minutes of utc offset as input
  int temp_minutes = O_S_minutes;
  while (true) {
    display.clearDisplay();
    print_line("Enter minutes: " + String(temp_minutes), 0, 0, 2);

    int button = wait_for_button_press();
    if (button == ok_PB) {
      delay(200);
      O_S_minutes = temp_minutes;
      break;
    }

    if (button == cancel_PB) {
      delay(200);
      break;
    }

    if (button == up_PB) {
      delay(200);
      temp_minutes += 1;
      temp_minutes %= 60;
    }

    else if (button == down_PB) {
      delay(200);
      temp_minutes -= 1;
      if (temp_minutes < 0) {
        temp_minutes += 60;
      }
    }
  }
  
  UTC_OFFSET = O_S_hours * 60 * 60 + O_S_minutes * 60; // Calculate utc offset in seconds
  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER); //Use setted utc offset and configure current time

  display.clearDisplay();
  print_line("Time zone is set", 0, 0, 2);
  delay(1000);
}

//*********************************************************************************************************************************


//***************************************************************************************************************************
void set_alarm(int alarm){
  int temp_hour = alarm_hours[alarm];
  while (true){
    display.clearDisplay();
    print_line("Enter hour: " + String(temp_hour), 0, 0, 2);

    int pressed = wait_for_button_press();
    if (pressed == up_PB){
      delay(200);
      temp_hour += 1;
      temp_hour = temp_hour % 24;     //after 25 count from 1
    }

    else if (pressed == down_PB){
      delay(200);
      temp_hour -= 1;
      if (temp_hour < 0){
        temp_hour = 23;    // 0 said the midnight 12 am
      }
    }
    
    else if (pressed == ok_PB){
      delay(200);
      alarm_hours[alarm] = temp_hour;
      break;
    }

    else if (pressed == cancel_PB){
      delay(200);
      break;
    }
  }

  int temp_minute = alarm_minutes[alarm];
  while(true){
    display.clearDisplay();
    print_line("Enter minute: " + String(temp_minute), 0, 0, 2);

    int pressed = wait_for_button_press();
    if (pressed == up_PB){
      delay(200);
      temp_minute += 1;
      temp_minute = temp_minute % 60;     //after 60 count from 1
    }

    else if (pressed == down_PB){
      delay(200);
      temp_minute -= 1;
      if (temp_minute < 0){
        temp_minute = 59;    // 0 said that was 60th minute 
      }
    }
    
    else if (pressed == ok_PB){
      delay(200);
      alarm_minutes[alarm] = temp_minute;
      break;
    }

    else if (pressed == cancel_PB){
      delay(200);
      break;
    }    
  }  

  display.clearDisplay();
  alarm_triggered[alarm]=false;
  print_line("Alarm is set", 0, 0, 2);
  delay(1000);
}
//**********************************************************************************************************************************



//**********************************************************************************************************************************
void play_danger(){
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, LOW);
  digitalWrite(LED_R, HIGH);
  tone(buzzer, D, 200);
  tone(buzzer,C, 200);
  delay(500);

  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, HIGH);
  tone(buzzer, D, 200);
  tone(buzzer, C, 200);
  delay(500);

  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, LOW);
}
//*************************************************************************************************************************************