//191123 fujigaya2
//210313 fujigaya2 for switch

#include <string.h>
//#include "Keyboard.h"
#include "DenshaMascon.h"
#include "SwitchControlLibrary.h"

Mascon Joystick;

#define EEP_ACC_RATE_DEFAULT 25
#define EEP_DEC_RATE_DEFAULT 15
#define EEP_COAST_RATE_DEFAULT 5

//USB SERIAL DEBUGをする場合はコメントアウトを取る
//#define USB_SERIAL_ENABLE

int joy_gear_mode = GEAR_MODE_SPD;//EEP_CONTROL_DEFAULT
int joy_accrate = EEP_ACC_RATE_DEFAULT;//50msecでどのくらい加速するか？２０４８が１００％の基準
int  joy_decrate = EEP_DEC_RATE_DEFAULT;
int  joy_coastrate = EEP_COAST_RATE_DEFAULT;
int prev_speed = 0;
unsigned int aPsxData = 0;
int gPreviousL6_wait = 50;

#define WAIT_KEY  333
#define WAIT_KEY_SPD  200
//Task Schedule
unsigned long gPreviousL5 = 250; //   50ms interval
unsigned long gPreviousL6 = 250; //   50ms interval

char aText[16 + 1];

void setup()
{
  // put your setup code here, to run once:
  //PSX
  Joystick.setupPins(MASCON_dataPin, MASCON_cmndPin, MASCON_attPin, MASCON_clockPin, 10);
#if defined(USB_SERIAL_ENABLE)
  Serial.begin(115200);
  Serial.println("Serial Begin.");
#endif
  SwitchControlLibrary();
}

void loop()
{
  // put your main code here, to run repeatedly:
  //Joystick
  if( (millis() - gPreviousL5) >= 50)
  {
    //Read joystick Status 
    aPsxData = Joystick.read();
    //検知用にバッファに入れておく
    Joystick.PutButtonBuffer(aPsxData);
    //Store status from aPsxData to gPower 
    Joystick.GetHandleStatus(aPsxData);
    //Read ButtonStatus and continueously push count    
    Joystick.GetButtonStatus(aPsxData);
    //Speed　→mode変えても変わらない？ 
    Joystick.Interval(joy_gear_mode ,joy_accrate,joy_decrate ,joy_coastrate );
    //Reset task
    gPreviousL5 = millis();
  }
  
  //buttonの確認
  if( (millis() - gPreviousL6) >= gPreviousL6_wait)
  {
    gPreviousL6_wait = 50;
    //まず全ボタンリリース(Left_Stick以外）
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
    SwitchControlLibrary().moveRightStick(128, 128);
    
    //SelectはShiftフラグとする。
    bool shift_flag = false;
    if((Joystick.GetButtonStatus(aPsxData) & JOYSTICK_SELECT) > 0)
    {
#if defined(USB_SERIAL_ENABLE)
      Serial.println("Select Button On.");
#endif
      shift_flag = true;
    }
    else
    {
      shift_flag = false;
    }
    
    if((Joystick.GetButtonStatus(aPsxData) & JOYSTICK_START) > 0)
    {
      if(shift_flag != true)
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
      }
    }
    else if((Joystick.GetButtonStatus(aPsxData) & JOYSTICK_A) > 0)  
    {
      if(shift_flag != true)
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
      }
    }
    else if((Joystick.GetButtonStatus(aPsxData) & JOYSTICK_B) > 0)  
    {
      if(shift_flag != true)
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
      }
    }
    else if((Joystick.GetButtonStatus(aPsxData) & JOYSTICK_C) > 0)  
    {
      if(shift_flag != true)
      {
#if defined(USB_SERIAL_ENABLE)
        Serial.println("C Button On.");
#endif
        SwitchControlLibrary().pressButton(Button::A);
      }
      else
      {
#if defined(USB_SERIAL_ENABLE)
        Serial.println("LEFT Button On.");
#endif
        SwitchControlLibrary().pressHatButton(HatButton::RIGHT);
      }
    }
    //ボタン系を一度送信
    SwitchControlLibrary().sendReport();

    //ノッチ系の確認
    //Left_Stickリセット
    SwitchControlLibrary().moveLeftStick(128, 128);

    switch(Joystick.GetHandleValue())
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
        break;
      case -8://B8
#if defined(USB_SERIAL_ENABLE)
        Serial.println("16 * 0 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 16 * 0);// 左スティックは上のほう
        break;
      case -7://B7
#if defined(USB_SERIAL_ENABLE)
        Serial.println("16 * 1 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 16 * 1); // 左スティックは上のほう
        break;
      case -6://B6
#if defined(USB_SERIAL_ENABLE)
        Serial.println("16 * 2 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 16 * 2); // 左スティックは上のほう
        break;
      case -5://B5
#if defined(USB_SERIAL_ENABLE)
        Serial.println("16 * 3 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 16 * 3); // 左スティックは上のほう
        break;
      case -4://B4
#if defined(USB_SERIAL_ENABLE)
        Serial.println("16 * 4 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 16 * 4); // 左スティックは上のほう
        break;
      case -3://B3
#if defined(USB_SERIAL_ENABLE)
        Serial.println("16 * 5 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 16 * 5); // 左スティックは上のほう
        break;
      case -2://B2
#if defined(USB_SERIAL_ENABLE)
        Serial.println("16 * 6 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 16 * 6); // 左スティックは上のほう
        break;
      case -1://B1
#if defined(USB_SERIAL_ENABLE)
        Serial.println("16 * 7 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 16 * 7); // 左スティックは上のほう
        break;
      case 0://N
#if defined(USB_SERIAL_ENABLE)
        Serial.println("16 * 8 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 16 * 8); // 左スティックは真ん中
        break;
      case 1://N
#if defined(USB_SERIAL_ENABLE)
        Serial.println("128 + 25 * 1 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 128 + 25 * 1); // 左スティックは下のほう
        break;
      case 2://N
#if defined(USB_SERIAL_ENABLE)
        Serial.println("128 + 25 * 2 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 128 + 25 * 2); // 左スティックは下のほう
        break;
      case 3://N
#if defined(USB_SERIAL_ENABLE)
        Serial.println("128 + 25 * 3 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 128 + 25 * 3); // 左スティックは下のほう
        break;
      case 4://N
#if defined(USB_SERIAL_ENABLE)
        Serial.println("128 + 25 * 4 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 128 + 25 * 4); // 左スティックは上のほう
        break;
      case 5://N
#if defined(USB_SERIAL_ENABLE)
        Serial.println("128 + 25 * 5 Stick On.");
#endif
        SwitchControlLibrary().moveLeftStick(128, 128 + 25 * 5); // 左スティックは下のほう
        break;
      default:
        break;
    }
    //ノッチ部分を送信
    SwitchControlLibrary().sendReport();    
    
    //Reset task
    gPreviousL6 = millis();
  }
}

