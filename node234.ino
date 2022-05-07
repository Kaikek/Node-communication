#include "namedMesh.h"
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define   MESH_SSID       "badeendjes"
#define   MESH_PASSWORD   "badeendjes"
#define   MESH_PORT       5555

Scheduler     userScheduler; // to control your personal task
namedMesh  mesh;

bool buttonState = false;

String nodeName = "node2"; // Name needs to be unique

Task taskSendMessage( TASK_SECOND * 1, TASK_FOREVER, []() {
  String msg = "";
  String to = "node3";
  if (digitalRead(18) == LOW) {
    if (buttonState == false)
    {
      msg = String("n");
      buttonState = true;
    }
    else
    {
      msg = String("y");
      buttonState = false;
    }
    mesh.sendSingle(to, msg);
    Serial.println(msg);
  }
}); // start with a one second interval

void setup() {
  Serial.begin(115200);

  mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION);  // set before init() so that you can see startup messages

  mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);

  mesh.setName(nodeName); // This needs to be an unique name!

  mesh.onReceive([](String & from, String & msg) {
    Serial.printf("Received message by name from: %s, %s\n", from.c_str(), msg.c_str());
    String message = String(msg).c_str();
    if (message == "y")
    {
      digitalWrite(4, HIGH);
    }
    else if (message == "n")
    {
      digitalWrite(4, LOW);
    }
    else
    {
      lcd.setCursor(0, 0);
      lcd.print("Temp: ");
      lcd.setCursor(8, 0);
      lcd.print(msg.c_str());
      lcd.setCursor(14,0);
      lcd.print((char)223);
      lcd.setCursor(15,0);
      lcd.print("C");
    }
  });

  mesh.onChangedConnections([]() {
    Serial.printf("Changed connection\n");
  });

  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();

  pinMode(18, INPUT_PULLUP);
  pinMode(4, OUTPUT);

  lcd.init();
  lcd.backlight();
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
}
