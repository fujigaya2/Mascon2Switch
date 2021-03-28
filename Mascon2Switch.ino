//191123 fujigaya2
//210313 fujigaya2 for switch

#include <string.h>
//#include "Keyboard.h"
#include "DenshaMascon.h"
#include "SwitchControlLibrary.h"

Mascon Joystick;


//USB SERIAL DEBUGをする場合はコメントアウトを取る
#define USB_SERIAL_ENABLE

int gPreviousL6_wait = 25;

//Select1wo押しっぱなしで、何サイクルでPLUSボタンを押すか(ms) = gPreviousL6_wait *SHIFT_COUNT_ON 
#define SHIFT_COUNT_ON  150

//Task Schedule
unsigned long gPreviousL5 = 250; //   50ms interval
unsigned long gPreviousL6 = 250; //   50ms interval


void button_release()
{
  //全ボタンリリース(Left_Stick以外）
  SwitchControlLibrary().releaseButton(Button::Y);
  SwitchControlLibrary().releaseButton(Button::B);
  SwitchControlLibrary().releaseButton(Button::A);
  SwitchControlLibrary().releaseButton(Button::X);
  SwitchControlLibrary().releaseButton(Button::L);
  SwitchControlLibrary().releaseButton(Button::R);
  SwitchControlLibrary().releaseButton(Button::ZL);
  SwitchControlLibrary().releaseButton(Button::ZR);
  SwitchControlLibrary().releaseButton(Button::MINUS);
  SwitchControlLibrary().releaseButton(Button::PLUS);
  SwitchControlLibrary().releaseButton(Button::LCLICK);
  SwitchControlLibrary().releaseButton(Button::RCLICK);
  SwitchControlLibrary().releaseButton(Button::HOME);
  SwitchControlLibrary().releaseButton(Button::CAPTURE);
  SwitchControlLibrary().releaseHatButton(HatButton::UP);//これで良いか？
  SwitchControlLibrary().releaseHatButton(HatButton::RIGHT);//これで良いか？
  SwitchControlLibrary().releaseHatButton(HatButton::DOWN);//これで良いか？
  SwitchControlLibrary().releaseHatButton(HatButton::LEFT);//これで良いか？
  
}

unsigned int read_joystick()
{
  //PS側のジョイスティックの状態を読む
  //Read joystick Status 
  unsigned int aPsxData = Joystick.read();
  //検知用にバッファに入れておく
  Joystick.PutButtonBuffer(aPsxData);
  //Store status from aPsxData to gPower 
  Joystick.GetHandleStatus(aPsxData);
  //Read ButtonStatus and continueously push count    
  Joystick.GetButtonStatus(aPsxData);
  return aPsxData;
}

int send_button(unsigned int aPsxData)
{
 
  //ボタンをスイッチ用に変換し、送信する。
  //SelectはShiftフラグとする。
  //Selectボタンのカウント 0:not pushed, not 0:pushed count
  static int shift_count = 0;
  if((Joystick.GetButtonStatus(aPsxData) & JOYSTICK_SELECT) > 0)
  {
    shift_count ++;
#if defined(USB_SERIAL_ENABLE)
      Serial.println("shift_count ++ ");
#endif
    if(shift_count > SHIFT_COUNT_ON)
    {
#if defined(USB_SERIAL_ENABLE)
      Serial.println("PLUS Button On.");
#endif
      SwitchControlLibrary().pressButton(Button::PLUS);
      shift_count = 0;
    }
  }
  else
  {
#if defined(USB_SERIAL_ENABLE)
      Serial.println("shift_count 0 ");
#endif
    shift_count = 0;
  }
  
  if((Joystick.GetButtonStatus(aPsxData) & JOYSTICK_START) > 0)
  {
    if(shift_count == 0)
    {
#if defined(USB_SERIAL_ENABLE)
      Serial.println("X Button On.");
#endif
      SwitchControlLibrary().pressButton(Button::X);
    }
    else
    {
#if defined(USB_SERIAL_ENABLE)
      Serial.println("LEFT Button On.");
#endif
      SwitchControlLibrary().pressHatButton(HatButton::UP);
      shift_count = 0;
    }
  }
  else if((Joystick.GetButtonStatus(aPsxData) & JOYSTICK_A) > 0)  
  {
    if(shift_count == 0)
    {
#if defined(USB_SERIAL_ENABLE)
      Serial.println("Y Button On.");
#endif
      SwitchControlLibrary().pressButton(Button::Y);
    }
    else
    {
#if defined(USB_SERIAL_ENABLE)
      Serial.println("RIGHT Button On.");
#endif
      SwitchControlLibrary().pressHatButton(HatButton::LEFT);
      shift_count = 0;
    }
  }
  else if((Joystick.GetButtonStatus(aPsxData) & JOYSTICK_B) > 0)  
  {
    if(shift_count == 0)
    {
#if defined(USB_SERIAL_ENABLE)
      Serial.println("B Button On.");
#endif
      SwitchControlLibrary().pressButton(Button::B);
    }
    else
    {
#if defined(USB_SERIAL_ENABLE)
      Serial.println("DOWN Button On.");
#endif
      SwitchControlLibrary().pressHatButton(HatButton::DOWN);
      shift_count = 0;
    }
  }
  else if((Joystick.GetButtonStatus(aPsxData) & JOYSTICK_C) > 0)  
  {
    if(shift_count == 0)
    {
#if defined(USB_SERIAL_ENABLE)
      Serial.println("C Button On.");
#endif
      SwitchControlLibrary().pressButton(Button::A);
    }
    else
    {
#if defined(USB_SERIAL_ENABLE)
      Serial.println("RIGHT Button On.");
#endif
      SwitchControlLibrary().pressHatButton(HatButton::RIGHT);
      shift_count = 0;
    }
  }
  //ボタン系を一度送信
  SwitchControlLibrary().sendReport();

  return shift_count;
  
}

void send_notch(int shift_count)
{
  //ノッチ系の確認

  int handle_pos = Joystick.GetHandleValue();
  // L,Rボタン機能を入れておく
  if(handle_pos <= -10 && shift_count != 0)
  {
#if defined(USB_SERIAL_ENABLE)
        Serial.println("R Button On.");
#endif
      SwitchControlLibrary().pressButton(Button::R);
  }
  if(handle_pos >= 5 && shift_count != 0)
  {
#if defined(USB_SERIAL_ENABLE)
        Serial.println("L Button On.");
#endif
      SwitchControlLibrary().pressButton(Button::L);
  }

  if(shift_count == 0)
  {
    switch(handle_pos)
    {
      case -1024://緊急ブレーキ
      case -160:
      case -800:
      case -40:
      case -20:
      case -10:
#if defined(USB_SERIAL_ENABLE)
        Serial.println("ZL Button On.");
#endif
        SwitchControlLibrary().pressButton(Button::ZL);
        SwitchControlLibrary().moveLeftStick(128, 128 - 16 * 8);
        SwitchControlLibrary().moveRightStick(128 + 16 * 8 - 1,128);
        break;
      case -8://B8
#if defined(USB_SERIAL_ENABLE)
        Serial.println("128 - 16 * 8 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 128 - 16 * 8);// 左スティックは上のほう
        SwitchControlLibrary().moveRightStick(128 + 16 * 8 - 1,128);
        break;
      case -7://B7
#if defined(USB_SERIAL_ENABLE)
        Serial.println("128 - 16 * 7 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 128 - 16 * 7); // 左スティックは上のほう
        SwitchControlLibrary().moveRightStick(128 + 16 * 7,128);
        break;
      case -6://B6
#if defined(USB_SERIAL_ENABLE)
        Serial.println("128 - 16 * 6 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 128 - 16 * 6); // 左スティックは上のほう
        SwitchControlLibrary().moveRightStick(128 + 16 * 6,128);
        break;
      case -5://B5
#if defined(USB_SERIAL_ENABLE)
        Serial.println("128 - 16 * 5 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 128 - 16 * 5); // 左スティックは上のほう
        SwitchControlLibrary().moveRightStick(128 + 16 * 5,128);
        break;
      case -4://B4
#if defined(USB_SERIAL_ENABLE)
        Serial.println("128 - 16 * 4.5 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 128 - 16 * 4.5); // 左スティックは上のほう
        SwitchControlLibrary().moveRightStick(128 + 16 * 4.5,128);
        break;
      case -3://B3
#if defined(USB_SERIAL_ENABLE)
        Serial.println("128 - 16 * 4 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 128 - 16 * 4); // 左スティックは上のほう
        SwitchControlLibrary().moveRightStick(128 + 16 * 4,128);
        break;
      case -2://B2//調整
#if defined(USB_SERIAL_ENABLE)
        Serial.println("128 - 16 * 3 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 128 - 16 * 3); // 左スティックは上のほう
        SwitchControlLibrary().moveRightStick(128 + 16 * 3,128);
        break;
      case -1://B1//調整
#if defined(USB_SERIAL_ENABLE)
        Serial.println("128 - 16 * 2 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 128 - 16 * 2); // 左スティックは上のほう
        SwitchControlLibrary().moveRightStick(128 + 16 * 2,128);
        break;
      case 0://N
#if defined(USB_SERIAL_ENABLE)
        Serial.println("128 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 128); // 左スティックは真ん中
        SwitchControlLibrary().moveRightStick(128,128);
        break;
      case 1://P1 //値調整
#if defined(USB_SERIAL_ENABLE)
        Serial.println("128 + 25 * 1.5 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 128 + 25 * 1.5); // 左スティックは下のほう
        SwitchControlLibrary().moveRightStick(128,128);
        break;
      case 2://P2
#if defined(USB_SERIAL_ENABLE)
        Serial.println("128 + 25 * 2 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 128 + 25 * 2); // 左スティックは下のほう
        SwitchControlLibrary().moveRightStick(128,128);
        break;
      case 3://P3
#if defined(USB_SERIAL_ENABLE)
        Serial.println("128 + 25 * 3 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 128 + 25 * 3); // 左スティックは下のほう
        SwitchControlLibrary().moveRightStick(128,128);
        break;
      case 4://P4
#if defined(USB_SERIAL_ENABLE)
        Serial.println("128 + 25 * 4 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 128 + 25 * 4); // 左スティックは上のほう
        SwitchControlLibrary().moveRightStick(128,128);
        break;
      case 5://P5
#if defined(USB_SERIAL_ENABLE)
        Serial.println("128 + 25 * 5 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 128 + 25 * 5); // 左スティックは下のほう
        SwitchControlLibrary().moveRightStick(128,128);
        break;
      default:
        break;
    }
  }
  //ノッチ部分を送信
  SwitchControlLibrary().sendReport();    

}
void setup()
{
  // put your setup code here, to run once:
  //PSX
  Joystick.setupPins(MASCON_dataPin, MASCON_cmndPin, MASCON_attPin, MASCON_clockPin, 10);
//#if defined(USB_SERIAL_ENABLE)
  Serial.begin(115200);
  Serial.println("Serial Begin.");
//#endif
  SwitchControlLibrary();

  button_release();
  SwitchControlLibrary().moveLeftStick(128, 128);
  SwitchControlLibrary().moveRightStick(128, 128);

}

void loop()
{
  static unsigned int aPsxData = 0;  
  // put your main code here, to run repeatedly:
  //Joystick
  if( (millis() - gPreviousL5) >= 25)
  {
    //Read Joystick state  
    aPsxData = read_joystick();
    //Reset task
    gPreviousL5 = millis();
  }

  
  //buttonの確認
  if( (millis() - gPreviousL6) >= gPreviousL6_wait)
  {
    gPreviousL6_wait = 25;

    button_release();
    int shift_count = send_button(aPsxData);
    send_notch(shift_count);
    
    //Reset task
    gPreviousL6 = millis();
  }
}
