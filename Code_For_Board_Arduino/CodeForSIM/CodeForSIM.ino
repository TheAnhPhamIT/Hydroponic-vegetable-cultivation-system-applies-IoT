/*      
 *      Khoa Luan Tot Nghiep - He Thong Trong Rau Thuy Canh Ung Dung IoT
 *      5/2019
 *      Code For Arduino Uno R3 Send To Module GSM SIM800A
 *      Written by TheAnhPham
 *      theanhphamit@gmail.com
 */

#include <SoftwareSerial.h>
#include <SerialCommand.h>

SerialCommand sCmd;
SoftwareSerial SIM800A(13,12);

void setup() {
 SIM800A.begin(115200);
 Serial.begin(115200);
 Serial.println ("SIM800A Ready");
 delay(100);
 sCmd.addCommand("DEN",SendChangeDenData);
 sCmd.addCommand("QUAT",SendChangeQuatData);
 sCmd.addCommand("BATBOM",SendChangeBatBomData);
 sCmd.addCommand("TATBOM", SendChangeTatBomData);
 sCmd.addCommand("TIME", SendChangeTime);
 sCmd.addCommand("LOGIN", login);
 sCmd.addCommand("RESETPASS", resetPass);
 sCmd.addCommand("LOSS_WIFI", sendLossWifi);
}

void SendChangeDenData(){
  SendMessageWithData("Gia tri cuong do anh sang bat den da thay doi: ");
}
void SendChangeTime(){
  SendMessageWithData("Gia tri thoi gian cam bien anh sang hoat dong da thay doi: ");
}
void SendChangeQuatData(){
  SendMessageWithData("Gia tri nhiet do bat quat da thay doi: ");
}

void SendChangeBatBomData(){
  SendMessageWithData("Gia tri thoi gian bom hoat dong da thay doi: ");
}
void SendChangeTatBomData(){
  SendMessageWithData("Gia tri thoi gian bom nghi da thay doi:");
}
void login(){
  SendMessageWithoutData("He thong dieu khien vua bi dang nhap");
}

void sendLossWifi(){
  SendMessageWithoutData("He thong da bi mat ket noi wifi");
}

void resetPass(){
  SendMessageWithData("Mat khau he thong da duoc doi thanh: ");
}

void loop() {
  sCmd.readSerial();
}


void EndConnection() {
  SIM800A.println("AT+CIPSHUT");//close the connection
  delay(100);
  ShowSerialData();

}

void ShowSerialData()
{
  while(SIM800A.available()!=0)
    Serial.write(SIM800A.read());
}
void SendMessageWithData(String event){
  char *arg;
  arg = sCmd.next();
  int value = atoi(arg); // Chuyển chuỗi thành số
  SendMessage(event,value);
}

void SendMessageWithoutData(String event){
  SendMessage(event,-1);
}
void SendMessage(String event, int value)
{
  Serial.println ("Sending Message");
  SIM800A.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(2000);
  Serial.println ("Set SMS Number");
  SIM800A.println("AT+CMGS=\"+84352429700\"\r"); //Mobile phone number to send message
  delay(2000);
  Serial.println ("Set SMS Content");
  if(value == -1){
     SIM800A.println(event);
  }
  else{
    SIM800A.print(event);
    SIM800A.println(value);
  }
  
  delay(1000);
  Serial.println ("Finish");
  SIM800A.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
  Serial.println ("Message has been sent ->SMS Selesai dikirim");
}


 void RecieveMessage()
{
  Serial.println ("SIM800A Membaca SMS");
  delay (1000);
  SIM800A.println("AT+CNMI=2,2,0,0,0"); // AT Command to receive a live SMS
  delay(10000);
  Serial.write ("Unread Message done");
 }
