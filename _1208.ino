#include <Bounce.h>  // 消除按鍵彈跳用
#include <Wire.h>    // 設定I2C使用
#include <LiquidCrystal_I2C.h>  // 使用LCD
#include <OneWire.h>            // 感溫線DS18B20
#include <DallasTemperature.h>  // 將數位資料轉換成溫度

float SetTemp = 58.0;       // 起始溫度設定值
int EnterKey = 1;        // 輸入的狀態
int RealyType = 1;       // 繼電器狀態
float CurrTemp = 0.0;    // 目前溫度

// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
// 設定 LCD I2C 位址
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 


Bounce bouncer = Bounce(4, 50);     // 增加溫度輸入為pin 4
Bounce bouncer1 = Bounce(3, 50);    // 降低溫度輸入為pin 3
Bounce bouncer2 = Bounce(2, 50);    // 確定輸入為pin 2
 
// Setup a oneWire instance to communicate with any OneWire devices 
// 溫度感測數位資料輸入為pin 6 
OneWire oneWire(6); 

// Pass our oneWire reference to Dallas Temperature.
// 將oneWire資料做轉換
DallasTemperature sensors(&oneWire);

void setup(){
  pinMode(2, INPUT);      //設定pin 2為輸入腳位
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, OUTPUT);     //設定pin 5為輸出腳位
  digitalWrite(5, HIGH);    //繼電器一開始輸出為高電位
   
  lcd.begin(16, 2);      // 初始化 LCD，一行 16 的字元，共 2 行

  // 閃爍三次
  for(int i = 0; i < 3; i++) {
    lcd.backlight();     //開啟背光
    delay(250);          //延遲0.25秒
    lcd.noBacklight();   //關閉背光
    delay(250);          
  }
  lcd.backlight();

  // 輸出初始化文字
  lcd.setCursor(0, 0);      // 設定游標位置在第一行行首
  lcd.print("Wellcome");    // LCD顯示預設文字
  delay(1500);
  lcd.setCursor(0, 1);      // 設定游標位置在第二行行首
  lcd.print("made:2018/12/08");
  delay(1500);
  lcd.clear();              // 清除LCD文字

  lcd.setCursor(0, 0); 
  lcd.print("Please set temp:");
  lcd.setCursor(0, 1);
  lcd.print(SetTemp);  
 
  sensors.begin();     //初始化感溫線

  while(EnterKey == 1) {     //一直循環,當EnterKey=0跳出
    while(bouncer.update() == true && bouncer.read() == LOW ) {  //判斷按鍵是否按下
      SetTemp+=0.5;              //按一下設定溫度加0.5度
      lcd.clear(); 
      lcd.setCursor(0, 0);  
      lcd.print("Please set temp:");
      lcd.setCursor(0, 1);
      lcd.print(SetTemp);  
    }
    while(bouncer1.update() == true && bouncer1.read() == LOW ) {
      SetTemp-=0.5;               //按一下設定溫度減0.5度
      lcd.clear();
      lcd.setCursor(0, 0); 
      lcd.print("Please set temp:");
      lcd.setCursor(0, 1);
      lcd.print(SetTemp);  
    }
    if(bouncer2.update() == true && bouncer2.read() == LOW ) {
      EnterKey=0;	//輸入完畢
    }
  }
}

void loop(){
  sensors.requestTemperatures();       // 要求獲得溫度資料
  CurrTemp=sensors.getTempCByIndex(0); // 感溫線序號為0,並轉換數位資料
    
  lcd.setCursor(0, 0); 
  lcd.print("Now Temp :"); 
  lcd.setCursor(11, 0); 
  lcd.print(CurrTemp); 
  lcd.setCursor(0, 1); 
  lcd.print("Set Temp :"); 
  lcd.setCursor(11, 1); 
  lcd.print(SetTemp); 
 
  if(RealyType == 1){              
    if (CurrTemp < (SetTemp-0.5)){    //判斷目前溫度是否到下限
      digitalWrite(5, LOW);        //繼電器給于低電壓
      RealyType=0;                 //改變繼電器狀態
    }
  }

  else if(RealyType == 0){
    if (CurrTemp > (SetTemp+0.5)){    //判斷目前溫度是否到上限
      digitalWrite(5, HIGH);
      RealyType=1;
    }
  }
}
