/*      
 *      Khoa Luan Tot Nghiep - He Thong Trong Rau Thuy Canh Ung Dung IoT
 *      5/2019
 *      Code For Mega 2650
 *      Written by TheAnhPham
 *      theanhphamit@gmail.com
 */
#include <SoftI2CMaster.h>
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <SerialCommand.h> // Thêm vào sketch thư viện Serial Command
#include <EEPROM.h>
#include <RTClib.h>
#include <BH1750.h>
#include <DS3231.h>

DS3231  rtc(20, 21); // Khởi tạo đối tượng rtc
Time t;

//Khai bao doi tuong cam bien cuong do anh sang
BH1750 lightMeter;

#define DHTTYPE DHT11
#define DHTPIN 10
#define LED 11
#define FAN 12
#define PUMP 13
#define socket 22
#define lossWifiPin 21
#define ASPIN 23

boolean startTime = false;

int OnHour = 6;//giờ bắt đầu 
int OnMin = 0;//phút bắt đầu
int UpTime = 11;//khoảng thời gian quang hợp

unsigned long time1 = 0;
unsigned long time2 = 0;
SerialCommand sCmd; // Khai báo biến sử dụng thư viện Serial Command
//SerialCommand sCmd2(Serial1);
DHT dht(DHTPIN, DHTTYPE);

//===== phan dinh nghia cac gia tri ========
float temp = 0;
float humi = 0;

int addrStoreLampData = 0;
int addrStoreFanData = 1;
int addrStorePumpWork = 2;
int addrStorePumpSleep = 3;
int addrStoreAsWork = 10;

int nguongBatQuat[2] = {25,35};
int nguongThoiGianAs[2] = {10,15};
int nguongBatDen[2] = {0,500};
int nguongBatBom[2] = {0,24};
int nguongTatBom[2] = {0,24};

boolean isSendPara = false;
boolean BomDone = false;
boolean isConfig = false;
unsigned long timeBom = 0;
unsigned long timeNghi = 0;

boolean isManual = false;

//phan dinh nghia khoa keypad
#define Password_Lenght 7
boolean lock = true;
boolean newPass = false;
char Data[Password_Lenght]; // 6 is the number of chars it can hold + the null char = 7
char Master[Password_Lenght]; 
byte data_count = 0, master_count = 0;

int *giaTriDieuChinh;
boolean trangThai = false;
char c;

int thoiGianBom = 23;
int thoiGianNghi = 0;

int thongSoBatDen = 500;
uint16_t cuongDoAnhSang = 0;

int thongSoBatQuat = 30;

boolean isControlSocket = false;

//======== phan dinh nghia cho ban phim ========
char key = 0;
const byte rows = 4; //so hang
const byte columns = 4; //so cot

//Dinh nghia cac gia tri tra ve
char keys[rows][columns]=
{
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'},
};

byte rowPins[rows] = {2,3,4,5}; // noi chan voi arduino
byte columnPins[columns] = {6,7,8,9};

//Khoi tao doi tuong keypad
Keypad keypad = Keypad(makeKeymap(keys),rowPins,columnPins,rows,columns);

//======= phan dinh nghia cho man hinh LCD ============
LiquidCrystal_I2C lcd(0x27,16,2);

void setup() {
   startTime = false;
   rtc.begin();
   EEPROM.begin();
   if(kiemTraGiaTri(int(EEPROM.read(addrStoreLampData)))){
    thongSoBatDen = int(EEPROM.read(addrStoreLampData));
   }
   if(kiemTraGiaTri(int(EEPROM.read(addrStoreFanData)))){
    thongSoBatQuat = int(EEPROM.read(addrStoreFanData));
   }
   if(kiemTraGiaTri(int(EEPROM.read(addrStorePumpWork)))){
    thoiGianBom = int(EEPROM.read(addrStorePumpWork));
   }
   if(kiemTraGiaTri((EEPROM.read(addrStorePumpSleep)))){
    thoiGianNghi = int(EEPROM.read(addrStorePumpSleep));
   }
   if(kiemTraGiaTri((EEPROM.read(addrStoreAsWork)))){
    UpTime = int(EEPROM.read(addrStoreAsWork));
   }
   if(kiemTraGiaTri(int(EEPROM.read(4)))==false){
   EEPROM.write(4,'0');delay(5);
   EEPROM.write(5,'0');delay(5);
   EEPROM.write(6,'0');delay(5);
   EEPROM.write(7,'0');delay(5);
   EEPROM.write(8,'0');delay(5);
   EEPROM.write(9,'0');delay(5);
   }
   
   Wire.begin();
   lightMeter.begin();
   pinMode(LED, OUTPUT);
   pinMode(FAN, OUTPUT);
   pinMode(PUMP, OUTPUT);
   pinMode(socket, OUTPUT);
   pinMode(ASPIN, OUTPUT);
   digitalWrite(ASPIN,HIGH);
   digitalWrite(lossWifiPin,LOW);
   
   Serial.begin(115200);// khởi tạo một cổng Serial tại baudrate 9600. Ở Client, cũng phải mở một cổng Serial cùng Baudrate với HOST
   Serial1.begin(115200);
   lightMeter.begin();
   sCmd.addCommand("LED",controlLED);
   sCmd.addCommand("FAN",controlFAN);
   sCmd.addCommand("PUMP", controlPUMP);
   sCmd.addCommand("control",onSocket);
   sCmd.addCommand("destroy",offSocket);
   //sCmd.addCommand("LOSS_WIFI", lossWifi);
   initLCD();
   //ok(trangThai);
   locked();
}
boolean kiemTraGiaTri(int value){
  if(value !=0 && value < 255)return true;
  return false;
}

void onSocket(){
  digitalWrite(socket,HIGH);
}

void offSocket(){
  digitalWrite(socket,LOW);
}
//ham tu dong bat tat den dua vao cuong do anh sang thuc te
void autoDen(){
  if(cuongDoAnhSang <= thongSoBatDen*10)digitalWrite(LED,HIGH);
  else digitalWrite(LED,LOW);
}
void autoQuat(){
  if(temp >= thongSoBatQuat)digitalWrite(FAN,HIGH);
   else digitalWrite(FAN,LOW);
}
//ham tang gia tri moi khi nhan nut ban phim
void tangGiaTri(int &giatri,int nguongTren){
  if(giatri < nguongTren){
    giatri+=1;
  }
}

//ham giam gia tri moi khi nhan nut ban phim
void giamGiaTri(int &giatri,int nguongDuoi){
  if(giatri > nguongDuoi){
     giatri-=1;
  }
}

// ham xem thong tin nhiet do va do am in ra man hinh
void xemThongTinCamBien(){
  //in thong so nhiet do ra man hinh
  lcd.setCursor(0,0);
  lcd.print("Temp  Humi  Lux");
  lcd.setCursor(0,1);
  lcd.print(temp);
  lcd.setCursor(6,1);
  lcd.print(humi);
  lcd.setCursor(12,1);
  lcd.print(cuongDoAnhSang);
}

void thietLapThoiGianAS(){
  lcd.setCursor(0,0);
  lcd.print("Thoi Gian AS:");
  lcd.setCursor(0,1);
  lcd.print(UpTime);
}

//ham thiet lap cac gia tri cho bom
void thietLapBom(){
  lcd.setCursor(0,0);lcd.print("TG Bom:");
  lcd.setCursor(7,0);lcd.print(thoiGianBom);
  lcd.setCursor(0,1);lcd.print("TG Nghi:");
  lcd.setCursor(8,1);lcd.print(thoiGianNghi);
}

//ham thiet lap cac gia tri cho den
void thietLapDen(){
  lcd.setCursor(0,0);lcd.print("Anh Sang Bat Den:");
  lcd.setCursor(0,1);lcd.print(thongSoBatDen);
}

//ham thiet lap cac gia tri cho quat
void thietLapQuat(){
  lcd.setCursor(0,0);lcd.print("Nhiet Do Bat Quat:");
  lcd.setCursor(0,1);lcd.print(thongSoBatQuat);
}
void ok(boolean &trangthai){
  trangthai = false;
  lcd.setCursor(3,0);lcd.print("Welcome To");
  lcd.setCursor(0,1);lcd.print("HT Thuy Canh IOT"); 
}
void initLCD(){
   lcd.init();
   lcd.backlight();
   lcd.display();
}

void clean(){
  lcd.clear();
}
//ham thay doi gia tri cua cac bien
void controlValue(int &giaTri,boolean &isConfig,int nguong[]){
  c = keypad.getKey();
  if((int)keypad.getState()== PRESSED){
    if(c != 0)key = c;
    if(key=='A'){tangGiaTri(giaTri,nguong[1]); delay(300);}
    if(key=='B'){giamGiaTri(giaTri,nguong[0]); delay(300);}
    if(key=='C'){
      isConfig = false;
    }
   clean();
  } 
}

//ham bat tat cac thiet bi
void controlTB(int pin, char name[]){
  lcd.setCursor(0,0);lcd.print("Dieu khien");
  lcd.setCursor(11,0);lcd.print(name);
  lcd.setCursor(0,1);
  if(digitalRead(pin)== HIGH){
    digitalWrite(pin,LOW);
    lcd.print("OFF");
  }
  else{
    digitalWrite(pin,HIGH);
    lcd.print("ON");
  }
}
// ham dieu khien den dua vao tin hieu gui tu websocket
void controlLED(){
  char *arg;
  arg = sCmd.next();
  
  int value = atoi(arg); // Chuyển chuỗi thành số
  digitalWrite(LED,value);
}

// ham dieu khien den dua vao tin hieu gui tu websocket
 void controlFAN(){
  char *arg;
  arg = sCmd.next();
  
  int value = atoi(arg); // Chuyển chuỗi thành số
  digitalWrite(FAN,value);
}

// ham dieu khien den dua vao tin hieu gui tu websocket
 void controlPUMP(){
  char *arg;
  arg = sCmd.next();
  
  int value = atoi(arg); // Chuyển chuỗi thành số
  digitalWrite(PUMP,value);
}
void locked(){
  lcd.setCursor(0,0);
  lcd.print("Enter Password");
}
void enterNewPass(){
  lcd.setCursor(0,0);
  lcd.print("Nhap Pass Moi");
}

/////////////////////////////////////////////////////////////////////////////
void loop() {
  
 //=========== phan lấy thời gian thực và bật tắt chế độ ánh sáng quang hợp ===========
  t = rtc.getTime();
  
  if((t.hour >= OnHour) && (t.hour < (t.hour + OnHour))){
    startTime = true;
  }
  if((t.hour - OnHour) >= UpTime){
    startTime = false;
  }
  //phan doc lenh tu serial
  
  if(isManual == false){
    sCmd.readSerial();
    
  }
 //========= doc gia tri tu cam bien va gui cho wemos =========
 int giaTriBanDau;
 if((unsigned long) (millis() - time1) > 2000){
    cuongDoAnhSang = lightMeter.readLightLevel();
    temp = dht.readTemperature();
    humi = dht.readHumidity();
  if (!isnan(temp) && !isnan(humi)) {
    Serial.println("temp "+String(temp,1));
    Serial.println("humi "+String(humi,1));
  }
  Serial.print("lux ");
  Serial.println(cuongDoAnhSang);
  time1 = millis();
  }
  if((unsigned long) (millis() - time2) > 10000){
    Serial.print("onQuat ");
    Serial.println(thongSoBatQuat);
    Serial.print("onDen ");
    Serial.println(thongSoBatDen);
    Serial.print("tgNghi ");
    Serial.println(thoiGianNghi);
    Serial.print("tgBom ");
    Serial.println(thoiGianBom);
    time2 = millis();
  }
  
//======== phan nhap mat khau dang nhap dieu khien he thong ==========
  c = keypad.getKey(); 
    if(lock == true && newPass == false){
      if (c) // makes sure a key is actually pressed, equal to (customKey != NO_KEY)
      {
        Data[data_count] = c; // store char into data array
        lcd.setCursor(data_count,1); // move cursor to show each new char
        lcd.print(Data[data_count]); // print char at said cursor
        data_count++; // increment data array by 1 to store new char, also keep track of the number of chars entered
      }
      if(data_count == Password_Lenght-1) // if the array index is equal to the number of expected chars, compare data to master
      {
        for(int i=4;i<10;i++){
          Master[i-4]= char(EEPROM.read(i));
        }
        delay(500);
        if(!strcmp(Data, Master)){
          lock = false;
          clean();
          ok(trangThai);
          data_count=0;
          Serial1.println("LOGIN");
        }else{
          clean();
          data_count = 0;
          locked();
        }
      }
    }
 //========== phan tao moi password ========
 
    if(newPass == true){
      if (c) // makes sure a key is actually pressed, equal to (customKey != NO_KEY)
      {
        Data[data_count] = c; // store char into data array
        lcd.setCursor(data_count,1); // move cursor to show each new char
        lcd.print(Data[data_count]); // print char at said cursor
        data_count++; // increment data array by 1 to store new char, also keep track of the number of chars entered
      }

      if(data_count == Password_Lenght-1) // if the array index is equal to the number of expected chars, compare data to master
      {
        for(int i=4;i<10;i++){
          EEPROM.write(i,Data[i-4]);delay(5);
        }
        Serial1.print("RESETPASS ");
        Serial1.println(Data);
        lock = true;
        data_count = 0;
        clean();
        delay(1000);
        locked();
        newPass = false;
      }
    }

   //============ phan dieu khien he thong bang ban phim =================
    if(lock == false && newPass == false){
    if((int)keypad.getState()== PRESSED){ 
      if(c != 0){
        key = c;
        if(key!='A'&& key!='B'){
          clean();
          isManual=true;
       }
        //if(key != 'C')
      if(isManual == true){
      switch(key){
      case '1':xemThongTinCamBien();break;
      case '2':controlTB(PUMP,"Bom");break;
      case '3':controlTB(LED,"Den");break;
      case '4':controlTB(FAN,"Quat");break;
      case '5': isConfig=true;
                giaTriBanDau=thoiGianBom;
                do{
                  thietLapBom();
                  controlValue(thoiGianBom,isConfig,nguongBatBom);
              }while(isConfig==true);
              if(giaTriBanDau != thoiGianBom){
                Serial1.print("BATBOM ");
                Serial1.println(thoiGianBom);
                EEPROM.write(addrStorePumpWork, thoiGianBom);
                delay(5);
              }
              break;
      case '6':isConfig=true;
                giaTriBanDau=thoiGianNghi;
                do{
                  thietLapBom();
                  controlValue(thoiGianNghi,isConfig,nguongTatBom);
               }while(isConfig==true);
               if(giaTriBanDau != thoiGianNghi){
                Serial1.print("TATBOM ");
                Serial1.println(thoiGianNghi);
                EEPROM.write(addrStorePumpSleep, thoiGianNghi);
                delay(5);
               }
              break;
      case '7':isConfig=true;
                giaTriBanDau = thongSoBatDen;
                do{
                  thietLapDen();
                  controlValue(thongSoBatDen,isConfig,nguongBatDen);
                }while(isConfig==true);
                if(giaTriBanDau != thongSoBatDen){
                  Serial1.print("DEN ");
                  Serial1.println(thongSoBatDen);
                  EEPROM.write(addrStoreLampData, thongSoBatDen);
                  delay(5);
                }
                 break;
      case '8':isConfig=true;
                giaTriBanDau = thongSoBatQuat;
              do{
                  thietLapQuat();
                  controlValue(thongSoBatQuat,isConfig,nguongBatQuat);
                }while(isConfig==true);
                if(giaTriBanDau != thongSoBatQuat){
                  Serial1.print("QUAT ");
                  Serial1.println(thongSoBatQuat);
                  EEPROM.write(addrStoreFanData, thongSoBatQuat);
                  delay(5);
                }
                break;
      case '*':isConfig=true;
                giaTriBanDau=UpTime;
                do{
                  thietLapThoiGianAS();
                  controlValue(UpTime,isConfig,nguongThoiGianAs);
              }while(isConfig==true);
              if(giaTriBanDau != UpTime){
                Serial1.print("TIME ");
                Serial1.println(UpTime);
                EEPROM.write(addrStoreAsWork, UpTime);
                delay(5);
              }
              break;
      case '0': Serial.println("resetWifi");break;
      case '#':lock=true;locked(); break;
      case 'D':newPass = true; enterNewPass();break;
      case 'C':ok(trangThai); isManual = false; timeBom=millis(); timeNghi = millis(); break;
      }
      }
      }
   }}
   
 //============== phan dieu khien tu dong ============
   if(isManual==false && digitalRead(socket)==LOW){
     if ((millis() - timeBom) <= thoiGianBom*1000){
      digitalWrite(PUMP,HIGH);
      if((millis() - timeBom) == thoiGianBom*1000)BomDone=true;
     }
     if (BomDone==true && ((millis() - timeNghi)<=(thoiGianNghi*1000 + thoiGianBom*1000))){
      digitalWrite(PUMP,LOW);
      if((millis()-timeNghi) == (thoiGianNghi*1000 + thoiGianBom*1000)){
        timeBom = millis();
        timeNghi = millis();
        BomDone=false;
      }
     }
    autoQuat();
    if(startTime == true){
      autoDen();
    }
   }
}
