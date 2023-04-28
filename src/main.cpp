#include <Arduino.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <esp_now.h>
#include <USB.h>
#include <USBHIDKeyboard.h>
#include <USBHIDConsumerControl.h>
#include <Preferences.h>

TFT_eSPI tft = TFT_eSPI();
USBHIDKeyboard Keyboard;
USBHIDConsumerControl cc;
Preferences pref;
TaskHandle_t TransmitTask;

uint8_t receiverAddress[] = {0xB8, 0xD6, 0x1A, 0x5D, 0x03, 0xBC};
esp_now_peer_info_t peerInfo;
String success;
bool rewriteMode = false;
typedef struct button_press
{
  String buttonLabel;
} button_press;

typedef struct buttons_send
{
  String buttons[12];
  int part = 0;
  int arrayLength;
} buttons_send;
typedef struct buttons_images_send
{
  String btnImages[12];
  int part = 1;
  int arrayLength;
} buttons_images_send;
typedef struct buttons_borders_send
{
  String borderColors[12];
  int arrayLength;
  int part = 2;
} buttons_borders_send;

typedef struct buttons_colors_send
{
  String btnColors[12];
  int part = 3;
  int arrayLength;
} buttons_colors_send;
button_press recvbtnPrsd;
button_press btnPrsd;
buttons_send btnsend;
buttons_colors_send btnclrsend;
buttons_images_send btnimgsend;
buttons_borders_send btnbordsend;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  tft.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&recvbtnPrsd, incomingData, sizeof(recvbtnPrsd));
  tft.print("Command: ");
  tft.println(recvbtnPrsd.buttonLabel);
  btnPrsd.buttonLabel = recvbtnPrsd.buttonLabel;
  if (btnPrsd.buttonLabel == "Copy")
  {
    Keyboard.begin();
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press('c');
    Keyboard.releaseAll();
    Keyboard.end();
  }
  else if (btnPrsd.buttonLabel == "Cut")
  {
    Keyboard.begin();
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press('x');
    Keyboard.releaseAll();
    Keyboard.end();
  }
  else if (btnPrsd.buttonLabel == "Paste")
  {
    Keyboard.begin();
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press('v');
    Keyboard.releaseAll();
    Keyboard.end();
  }
  else if (btnPrsd.buttonLabel == "Previous")
  {
    cc.begin();
    cc.press(CONSUMER_CONTROL_SCAN_PREVIOUS);
    cc.release();
    cc.end();
  }
  else if (btnPrsd.buttonLabel == "Play/Pause")
  {
    cc.begin();
    cc.press(CONSUMER_CONTROL_PLAY_PAUSE);
    cc.release();
    cc.end();
  }
  else if (btnPrsd.buttonLabel == "Next")
  {
    cc.begin();
    cc.press(CONSUMER_CONTROL_SCAN_NEXT);
    cc.release();
    cc.end();
  }
  else if (btnPrsd.buttonLabel == "Esc")
  {
    Keyboard.begin();
    Keyboard.press(KEY_ESC);
    Keyboard.releaseAll();
    Keyboard.end();
  }
  else if (btnPrsd.buttonLabel == "Enter")
  {
    Keyboard.begin();
    Keyboard.press(KEY_RETURN);
    Keyboard.releaseAll();
    Keyboard.end();
  }
  else if (btnPrsd.buttonLabel == "Up")
  {
    Keyboard.begin();
    Keyboard.press(KEY_UP_ARROW);
    Keyboard.releaseAll();
    Keyboard.end();
  }
  else if (btnPrsd.buttonLabel == "Down")
  {
    Keyboard.begin();
    Keyboard.press(KEY_DOWN_ARROW);
    Keyboard.releaseAll();
    Keyboard.end();
  }
  else if (btnPrsd.buttonLabel == "Left")
  {
    Keyboard.begin();
    Keyboard.press(KEY_LEFT_ARROW);
    Keyboard.releaseAll();
    Keyboard.end();
  }
  else if (btnPrsd.buttonLabel == "Right")
  {
    Keyboard.begin();
    Keyboard.press(KEY_RIGHT_ARROW);
    Keyboard.releaseAll();
    Keyboard.end();
  }
  else if (btnPrsd.buttonLabel == "Volume Up")
  {
    cc.begin();
    cc.press(CONSUMER_CONTROL_VOLUME_INCREMENT);
    cc.release();
    cc.end();
  }
  else if (btnPrsd.buttonLabel == "Volume Down")
  {
    cc.begin();
    cc.press(CONSUMER_CONTROL_VOLUME_DECREMENT);
    cc.release();
    cc.end();
  }
  else if (btnPrsd.buttonLabel == "Mute")
  {
    cc.begin();
    cc.press(CONSUMER_CONTROL_MUTE);
    cc.release();
    cc.end();
  }
  else if (btnPrsd.buttonLabel == "Brush")
  {
    Keyboard.begin();
    Keyboard.press('b');
    Keyboard.releaseAll();
    Keyboard.end();
  }
  else if (btnPrsd.buttonLabel == "Comment")
  {
    Keyboard.begin();
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press('/');
    Keyboard.releaseAll();
    Keyboard.end();
  }
  else if (btnPrsd.buttonLabel == "Run")
  {
    Keyboard.begin();
    Keyboard.press(KEY_F5);
    Keyboard.releaseAll();
    Keyboard.end();
  }
  else if (btnPrsd.buttonLabel == "Stop")
  {
    Keyboard.begin();
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_F5);
    Keyboard.releaseAll();
    Keyboard.end();
  }
  else if (btnPrsd.buttonLabel == "Debug")
  {
    Keyboard.begin();
    Keyboard.press(KEY_LEFT_SHIFT);
    Keyboard.press(KEY_F5);
    Keyboard.releaseAll();
    Keyboard.end();
  }
  
  else if (btnPrsd.buttonLabel.indexOf("ctrl") > -1 || btnPrsd.buttonLabel.indexOf("alt") > -1 || btnPrsd.buttonLabel.indexOf("shift") > -1 || btnPrsd.buttonLabel.indexOf("meta") > -1)
  {
    Keyboard.begin();
    if (btnPrsd.buttonLabel.indexOf("ctrl") > -1)
    {
      Keyboard.press(KEY_LEFT_CTRL);
      btnPrsd.buttonLabel.replace("ctrl", "");
    }
    if (btnPrsd.buttonLabel.indexOf("alt") > -1)
    {
      Keyboard.press(KEY_LEFT_ALT);
      btnPrsd.buttonLabel.replace("alt", "");
    }
    if (btnPrsd.buttonLabel.indexOf("shift") > -1)
    {
      Keyboard.press(KEY_LEFT_SHIFT);
      btnPrsd.buttonLabel.replace("shift", "");
    }
    if (btnPrsd.buttonLabel.indexOf("meta") > -1)
    {
      Keyboard.press(KEY_LEFT_GUI);
      btnPrsd.buttonLabel.replace("meta", "");
    }
    if (btnPrsd.buttonLabel.indexOf("+") + 1 == btnPrsd.buttonLabel.length() - 1)
    {
      Keyboard.press(btnPrsd.buttonLabel.charAt(btnPrsd.buttonLabel.indexOf("+") + 1));
    }
      if (btnPrsd.buttonLabel.indexOf("delete") > -1)
      {
        Keyboard.press(KEY_DELETE);
      }
      else if (btnPrsd.buttonLabel.indexOf("insert") > -1)
      {
        Keyboard.press(KEY_INSERT);
      }
      else if (btnPrsd.buttonLabel.indexOf("home") > -1)
      {
        Keyboard.press(KEY_HOME);
      }
      else if (btnPrsd.buttonLabel.indexOf("pageup") > -1)
      {
        Keyboard.press(KEY_PAGE_UP);
      }
      else if (btnPrsd.buttonLabel.indexOf("pagedown") > -1)
      {
        Keyboard.press(KEY_PAGE_DOWN);
      }
      else if (btnPrsd.buttonLabel.indexOf("end") > -1)
      {
        Keyboard.press(KEY_END);
      }
      else if (btnPrsd.buttonLabel.indexOf("F1") > -1)
      {
        Keyboard.press(KEY_F1);
      }
      else if (btnPrsd.buttonLabel.indexOf("F2") > -1)
      {
        Keyboard.press(KEY_F2);
      }
      else if (btnPrsd.buttonLabel.indexOf("F3") > -1)
      {
        Keyboard.press(KEY_F3);
      }
      else if (btnPrsd.buttonLabel.indexOf("F4") > -1)
      {
        Keyboard.press(KEY_F4);
      }
      else if (btnPrsd.buttonLabel.indexOf("F5") > -1)
      {
        Keyboard.press(KEY_F5);
      }
      else if (btnPrsd.buttonLabel.indexOf("F6") > -1)
      {
        Keyboard.press(KEY_F6);
      }
      else if (btnPrsd.buttonLabel.indexOf("F7") > -1)
      {
        Keyboard.press(KEY_F7);
      }
      else if (btnPrsd.buttonLabel.indexOf("F8") > -1)
      {
        Keyboard.press(KEY_F8);
      }
      else if (btnPrsd.buttonLabel.indexOf("F9") > -1)
      {
        Keyboard.press(KEY_F9);
      }
      else if (btnPrsd.buttonLabel.indexOf("F10") > -1)
      {
        Keyboard.press(KEY_F10);
      }
      else if (btnPrsd.buttonLabel.indexOf("F11") > -1)
      {
        Keyboard.press(KEY_F11);
      }
      else if (btnPrsd.buttonLabel.indexOf("F12") > -1)
      {
        Keyboard.press(KEY_F12);
      }
      else if (btnPrsd.buttonLabel.indexOf("tab") > -1)
      {
        Keyboard.press(KEY_TAB);
      }
      else if (btnPrsd.buttonLabel.indexOf("enter") > -1)
      {
        Keyboard.press(KEY_RETURN);
      }
    Keyboard.releaseAll();
    Keyboard.end();
  }
  else if (btnPrsd.buttonLabel.indexOf("key") > -1)
  {
    Keyboard.begin();
    if (btnPrsd.buttonLabel.indexOf("delete") > -1)
    {
      Keyboard.press(KEY_DELETE);
    }
    else if (btnPrsd.buttonLabel.indexOf("insert") > -1)
    {
      Keyboard.press(KEY_INSERT);
    }
    else if (btnPrsd.buttonLabel.indexOf("home") > -1)
    {
      Keyboard.press(KEY_HOME);
    }
    else if (btnPrsd.buttonLabel.indexOf("pageup") > -1)
    {
      Keyboard.press(KEY_PAGE_UP);
    }
    else if (btnPrsd.buttonLabel.indexOf("pagedown") > -1)
    {
      Keyboard.press(KEY_PAGE_DOWN);
    }
    else if (btnPrsd.buttonLabel.indexOf("end") > -1)
    {
      Keyboard.press(KEY_END);
    }
    else if (btnPrsd.buttonLabel.indexOf("F1") > -1)
    {
      Keyboard.press(KEY_F1);
    }
    else if (btnPrsd.buttonLabel.indexOf("F2") > -1)
    {
      Keyboard.press(KEY_F2);
    }
    else if (btnPrsd.buttonLabel.indexOf("F3") > -1)
    {
      Keyboard.press(KEY_F3);
    }
    else if (btnPrsd.buttonLabel.indexOf("F4") > -1)
    {
      Keyboard.press(KEY_F4);
    }
    else if (btnPrsd.buttonLabel.indexOf("F5") > -1)
    {
      Keyboard.press(KEY_F5);
    }
    else if (btnPrsd.buttonLabel.indexOf("F6") > -1)
    {
      Keyboard.press(KEY_F6);
    }
    else if (btnPrsd.buttonLabel.indexOf("F7") > -1)
    {
      Keyboard.press(KEY_F7);
    }
    else if (btnPrsd.buttonLabel.indexOf("F8") > -1)
    {
      Keyboard.press(KEY_F8);
    }
    else if (btnPrsd.buttonLabel.indexOf("F9") > -1)
    {
      Keyboard.press(KEY_F9);
    }
    else if (btnPrsd.buttonLabel.indexOf("F10") > -1)
    {
      Keyboard.press(KEY_F10);
    }
    else if (btnPrsd.buttonLabel.indexOf("F11") > -1)
    {
      Keyboard.press(KEY_F11);
    }
    else if (btnPrsd.buttonLabel.indexOf("F12") > -1)
    {
      Keyboard.press(KEY_F12);
    }
    else if (btnPrsd.buttonLabel.indexOf("tab") > -1)
    {
      Keyboard.press(KEY_TAB);
    }
    else if (btnPrsd.buttonLabel.indexOf("enter") > -1)
    {
      Keyboard.press(KEY_RETURN);
    }
    else
    {
      Keyboard.press(btnPrsd.buttonLabel.charAt(btnPrsd.buttonLabel.indexOf("key") + 3));
    }
    Keyboard.releaseAll();
    Keyboard.end();
  }
}
void TransmitTaskCode(void *parameter)
{
  for (;;)
  {
    
  }
}
uint16_t getRGBFromString(String rgbstr) {
  int arraySize = 3;          // Number of elements in the array
  String stringArray[arraySize]; // Declare the String array with a fixed size
  int index = 0;           // Index for the String array

  // Split the inputString with the delimiter ","
  while (rgbstr.length() > 0 && index < arraySize)
  {
    int commaIndex = rgbstr.indexOf(",");
    if (commaIndex >= 0)
    {
      stringArray[index] = rgbstr.substring(0, commaIndex);
      rgbstr = rgbstr.substring(commaIndex + 1);
    }
    else
    {
      stringArray[index] = rgbstr;
      rgbstr = "";
    }
    index++;
  }
  return tft.color565(stringArray[0].toInt(), stringArray[1].toInt(), stringArray[2].toInt());
}
void setup()
{
  pref.begin("rewriteMode");
  tft.begin();
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  WiFi.mode(WIFI_STA);
  rewriteMode = pref.getBool("rewriteMode");

  String tempbtns[12] = {"Copy", "Cut", "Paste", "Previous", "Play/Pause", "Next", "Volume Down", "Mute", "Volume Up", "Comment", "Esc", "Enter"};
  String tempbtnicons[12] = {"Copy", "Cut", "Paste", "Previous", "Playpause", "Next", "Voldown", "Mute", "VolUp", "Comment", "Escape", "Enter"};
  String tempcols[12] = {"0,255,255", "255,255,0", "0,255,0", "255,0,0", "0,255,0", "0,0,255", "0,255,255", "0,255,255", "0,255,255", "0,255,255", "0,255,255", "0,255,255"};
  String tempbordcols[12] = {"0,215,215", "215,215,0", "0,215,0", "215,0,0", "0,215,0", "0,0,215", "0,215,215", "0,215,215", "0,215,215", "0,215,215", "0,215,215", "0,215,215"};
  for (int i = 0; i < 12; i++)
  {
    tempbtns[i] = pref.getString(("btnfunc" + String(i)).c_str(), "Copy");
    tempbtnicons[i] = pref.getString(("btnimg" + String(i)).c_str(), "Copy");
    tempcols[i] = pref.getString(("btnbg" + String(i)).c_str(), "255,0,0");
    tempbordcols[i] = pref.getString(("btnbord" + String(i)).c_str(), "215,0,0");
  }
  for (int i = 0; i < 12; i++)
  {
    btnsend.buttons[i] = tempbtns[i];
    btnimgsend.btnImages[i] = tempbtnicons[i];
    btnclrsend.btnColors[i] = tempcols[i];
    btnbordsend.borderColors[i] = tempbordcols[i];
  }
  btnsend.arrayLength = 12;
  btnclrsend.arrayLength = 12;

  if (!rewriteMode)
  {
    if (esp_now_init() != ESP_OK)
    {
      tft.setTextColor(TFT_RED);
      tft.setCursor(0,0);
      tft.setTextSize(2);
      tft.println("Error initializing ESP-NOW");
      tft.setTextColor(TFT_WHITE);
      return;
    }
    esp_now_register_send_cb(OnDataSent);
    memcpy(peerInfo.peer_addr, receiverAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = true;

    // Add peer
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
      tft.setTextColor(TFT_RED);
      tft.setCursor(0, 0);
      tft.setTextSize(2);
      tft.println("Failed to add peer");
      tft.setTextColor(TFT_WHITE);
      return;
    }
    esp_now_register_recv_cb(OnDataRecv);
    USB.usbClass(0);
    USB.usbSubClass(0);
    USB.usbProtocol(0);
    USB.begin();
    tft.setTextColor(TFT_GREEN);
    tft.println("Ready!");
    tft.setTextColor(TFT_WHITE);
  }
  else
  {
    Serial.begin(115200);
    tft.setTextColor(TFT_YELLOW);
    tft.println("Awaiting Serial Init");
    tft.setTextColor(TFT_WHITE);
    while (!Serial)
    {
    }
    xTaskCreatePinnedToCore(
        TransmitTaskCode, /* Function to implement the task */
        "Task1",          /* Name of the task */
        10000,            /* Stack size in words */
        NULL,             /* Task input parameter */
        0,                /* Priority of the task */
        &TransmitTask,    /* Task handle. */
        0);               /* Core where the task should run */
    tft.drawWideLine(0, 0, tft.width(), tft.height(), 5, TFT_RED, TFT_TRANSPARENT);
    tft.drawWideLine(tft.width(), 0, 0, tft.height(), 5, TFT_RED, TFT_TRANSPARENT);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.drawCentreString("REWRITE MODE", tft.width() / 2, 0, 1);
  }
}
void reportError(esp_err_t result) {
  if (result == ESP_OK)
  {
    tft.println("Sent with success");
  }
  else if (result == ESP_ERR_ESPNOW_NOT_INIT)
  {
    tft.println("ESP-NOW: was not init");
  }
  else if (result == ESP_ERR_ESPNOW_ARG)
  {
    if (sizeof(btnsend) > 250)
    {
      tft.print("ESP-NOW: payload too large: ");
      tft.println(sizeof(btnsend));
    }
    else
    {
      tft.println("ESP-NOW: invalid argument");
    }
  }
  else if (result == ESP_ERR_ESPNOW_INTERNAL)
  {
    tft.println("ESP-NOW: internal error");
  }
  else if (result == ESP_ERR_ESPNOW_NO_MEM)
  {
    tft.println("ESP-NOW: no memory");
  }
  else if (result == ESP_ERR_ESPNOW_NOT_FOUND)
  {
    tft.println("ESP-NOW: peer not found");
  }
  else if (result == ESP_ERR_ESPNOW_IF)
  {
    tft.println("ESP-NOW: wifi interface doesn't match peer");
  }

  else
  {
    tft.print("Error sending the data, err: ");
    tft.println(result);
  }
}
void loop()
{
  if (!rewriteMode)
  {
    if (digitalRead(14) == LOW)
    {
      // put your main code here, to run repeatedly:
      // Send message via ESP-NOW
      String tempbtns[12] = {"Copy", "Cut", "Paste", "Previous", "Play/Pause", "Next", "Volume Down", "Mute", "Volume Up", "Comment", "Esc", "Enter"};
      String tempbtnicons[12] = {"Copy", "Cut", "Paste", "Previous", "Play/Pause", "Next", "Volume Down", "Mute", "Volume Up", "Comment", "Escape", "Enter"};
      String tempcols[12] = {"0,255,255", "255,255,0", "0,255,0", "255,0,0", "0,255,0", "0,0,255", "0,255,255", "0,255,255", "0,255,255", "0,255,255", "0,255,255", "0,255,255"};
      String tempbordcols[12] = {"0,215,215", "215,215,0", "0,215,0", "215,0,0", "0,215,0", "0,0,215", "0,215,215", "0,215,215", "0,215,215", "0,215,215", "0,215,215", "0,215,215"};
      for (int i = 0; i < 12; i++)
      {
        tempbtns[i] = pref.getString(("btnfunc" + String(i)).c_str(), "Copy");
        tempbtnicons[i] = pref.getString(("btnimg" + String(i)).c_str(), "Copy");
        tempcols[i] = pref.getString(("btnbg" + String(i)).c_str(), "255,0,0");
        tempbordcols[i] = pref.getString(("btnbord" + String(i)).c_str(), "215,0,0");
        btnsend.buttons[i] = tempbtns[i];
        btnimgsend.btnImages[i] = tempbtnicons[i];
        btnbordsend.borderColors[i] = tempbordcols[i];
        btnclrsend.btnColors[i] = tempcols[i];
      }
      btnsend.arrayLength = 12;
      btnclrsend.arrayLength = 12;
      esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)&btnsend, sizeof(btnsend));
      reportError(result);
      esp_err_t result2 = esp_now_send(receiverAddress, (uint8_t *)&btnimgsend, sizeof(btnimgsend));
      reportError(result2);
      esp_err_t result3 = esp_now_send(receiverAddress, (uint8_t *)&btnclrsend, sizeof(btnclrsend));
      reportError(result3);
      esp_err_t result4 = esp_now_send(receiverAddress, (uint8_t *)&btnbordsend, sizeof(btnbordsend));
      reportError(result4);
      
      delay(200);
    }
    if (digitalRead(0) == LOW)
    {
      pref.putBool("rewriteMode", true);
      ESP.restart();
    }
  }
  else {
    String result = Serial.readString();
    if(result == "REQUESTING BUTTONS\r\n")
    {
      tft.println("RETURNING");
      Serial.println("RETURNING");
      delay(1000);
      for (int i = 0; i < btnsend.arrayLength; i++)
      {
        String btndata = String(i) + "|" + btnclrsend.btnColors[i] + "|" + btnbordsend.borderColors[i] + "|" + btnimgsend.btnImages[i] + "|" + btnsend.buttons[i];
        Serial.println(btndata);
        tft.setTextSize(1);
        tft.println(btnsend.buttons[i]);
      }
      Serial.println("DONE");
    }
    else if(result == "EXIT\r\n") {
      pref.putBool("rewriteMode", false);
      ESP.restart();
    }
    else if(result == "TRANSMISSION READY\r\n") {
      bool syncError = false;
      Serial.println("BEGIN TRANSMISSION");
      tft.println("AWAITING...");
      for (int i = 0; i < 12; i++)
      {
        tft.println(i);
        String response = "";
        while (true)
        {
          if (Serial.available())
          {
            response = Serial.readString();
            // tft.println(response);
            break;
          }
        }
        int arraySize = 5;             // Number of elements in the array
        String stringArray[arraySize]; // Declare the String array with a fixed size
        int index = 0;                 // Index for the String array

        // Split the inputString with the delimiter ","
        while (response.length() > 0 && index < arraySize)
        {
          int commaIndex = response.indexOf("|");
          if (commaIndex >= 0)
          {
            stringArray[index] = response.substring(0, commaIndex);
            response = response.substring(commaIndex + 1);
          }
          else
          {
            stringArray[index] = response;
          }
          index++;
        }
        for (int part = 0; part < 5; part++)
        {
          tft.print("Part " + String(part) + ": ");
          String section = "";
          section = stringArray[part];
          tft.setTextColor(TFT_GREEN, TFT_BLACK);
          tft.println(section);
          tft.setTextColor(TFT_WHITE, TFT_BLACK);
          if (part == 0)
          {
            if (section.toInt() != i)
            {
              syncError = true;
              tft.setCursor(0,0);
              tft.setTextColor(TFT_RED, TFT_BLACK);
              tft.println("SYNC ERROR!!");
              tft.println("Current: " + String(i));
              tft.println("Remote: " + section);
              delay(1500);
              ESP.restart();
            }
          }
          else if (part == 1)
          {
            btnclrsend.btnColors[i] = section;
          }
          else if (part == 2)
          {
            btnbordsend.borderColors[i] = section;
          }
          else if (part == 3)
          {
            btnimgsend.btnImages[i] = section;
          }
          else if (part == 4)
          {
            section.replace("\r\n", "");
            btnsend.buttons[i] = section;
          }
        }
        pref.putString(("btnbg" + String(i)).c_str(), btnclrsend.btnColors[i]);
        pref.putString(("btnbord" + String(i)).c_str(), btnbordsend.borderColors[i]);
        pref.putString(("btnimg" + String(i)).c_str(), btnimgsend.btnImages[i]);
        pref.putString(("btnfunc" + String(i)).c_str(), btnsend.buttons[i]);
      }
      tft.fillScreen(TFT_BLACK);
      tft.setCursor(0, 0);
      tft.println("Process Completed Successfully");
      tft.println("System will reset and exit rewrite mode, once back in normal mode, press right to send new buttons");
      tft.println("Resetting in 3 seconds...");
      delay(3000);
      pref.putBool("rewriteMode", false);
      ESP.restart();
    }
  }
}