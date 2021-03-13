/*  PSX Controller Decoder Library (DenshaMascon.cpp)
	Written by: Kevin Ahrendt June 22nd, 2008
	Modified by: Yaasan / Desktop Station Dec 15, 2015
	
	Controller protocol implemented using Andrew J McCubbin's analysis.
	http://www.gamesx.com/controldata/psxcont/psxcont.htm
	
	Shift command is based on tutorial examples for ShiftIn and ShiftOut
	functions both written by Carlyn Maw and Tom Igoe
	http://www.arduino.cc/en/Tutorial/ShiftIn
	http://www.arduino.cc/en/Tutorial/ShiftOut

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*  
    
	E 1000000000001111, 1111000000001111
	8 1011000000001111, 1111000000001111
	7 1001000000001111, 1111000000001111
	6 0110000000001111, 1111000000001111
	5 0100000000001111, 1111000000001111
	4 0110000000001111, 1111000000001111
	3 1100000000001111, 1111000000001111
	2 1110000000001111, 1111000000001111
	1 0111000000001010, 1111000000001111
	N 0111000000001111, 1111000000001111
	1 1101100000001110, 1101000000001010
	2 1101100000001110, 1101000000001010
	3 1101000000001110, 1101000000001010
	4 1101100000001011, 1101000000001010, 
	5 1101000000001011, 1101000000001010

	       1101000000001010
	SELECT ........x.......
	START  ...........x....
	A      .......x
	B      ......x
	C      .....x
	MASK   zzzzz.......zzzz
    
    
*/

#include "DenshaMascon.h"

byte Mascon::shift(byte _dataOut)							// Does the actual shifting, both in and out simultaneously
{
	_temp = 0;
	_dataIn = 0;

	for (_i = 0; _i <= 7; _i++)
	{
		
		
		if ( _dataOut & (1 << _i) ) digitalWrite(_cmndPin, HIGH);	// Writes out the _dataOut bits
		else digitalWrite(_cmndPin, LOW);

		digitalWrite(_clockPin, LOW);
		
		delayMicroseconds(_delay);

		_temp = digitalRead(_dataPin);					// Reads the data pin
		if (_temp)
		{
			_dataIn = _dataIn | (B10000000 >> _i);		// Shifts the read data into _dataIn
		}

		digitalWrite(_clockPin, HIGH);
		delayMicroseconds(_delay);
	}
	return _dataIn;
}


void Mascon::setupPins(byte dataPin, byte cmndPin, byte attPin, byte clockPin, byte delay)
{
	pinMode(dataPin, INPUT);
	digitalWrite(dataPin, HIGH);	// Turn on internal pull-up
	_dataPin = dataPin;

	pinMode(cmndPin, OUTPUT);
	_cmndPin = cmndPin;

	pinMode(attPin, OUTPUT);
	_attPin = attPin;
	digitalWrite(_attPin, HIGH);

	pinMode(clockPin, OUTPUT);
	_clockPin = clockPin;
	digitalWrite(_clockPin, HIGH);
	
	_delay = delay;
}


unsigned int Mascon::read()
{
	digitalWrite(_attPin, LOW);

	shift(0x01);
	shift(0x42);
	shift(0xFF);

	_data1 = ~shift(0xFF);
	_data2 = ~shift(0xFF);

	digitalWrite(_attPin, HIGH);

	_dataOut = (_data2 << 8) | _data1;
  //Serial.println(_dataOut,HEX);
	return _dataOut;
}


byte Mascon::Interval(int mode,int acc_rate,int dec_rate,int coast_rate)
{
  //161105 fujigaya2
	int aSpeedRef;
	byte aRet = 0;

  //SpeedRefは元々±2047としている
  //→これを10倍して使うことことにする
  
  //gPowerが＋　加速
  //gPowerが―　減速
  //gPowerが０  惰行

  
  if(gPower == -1024)
  {
    //emergency brake
    aSpeedRef = 0;
  }
  else if(gPower < 0)
  {
    //Decel
    aSpeedRef = gSpeedRef + (gPower * dec_rate);
  }
  else if (gPower > 0)
  {
    //Accel
    for(int j = 0;j < 5;j++)
    {
      int gPower_rate = 0;
      if((gPower - j) > 0 ) 
      {
        gPower_rate = gPower - j;
      }
      if(gSpeedRef < gAccData[j] * 10)
      {
        aSpeedRef = gSpeedRef + gPower_rate * acc_rate;
        break;    
      }
      else
      {
        aSpeedRef = gSpeedRef;
      }
    }

  }
  else
  {
    //gPower ==0
     aSpeedRef = gSpeedRef - coast_rate;
  }

  //Limit
  if( aSpeedRef < 0)
  {
    aSpeedRef = 0;
  }
  else if ( aSpeedRef > 20470)
  {
    aSpeedRef = 20470;
  }
  
  if( (aSpeedRef >> 1) != (gSpeedRef >> 1))
  {
    aRet = 1;
  }
	
	gSpeedRef = aSpeedRef;
  
	return aRet;
	
}

word Mascon::GetSpeedValue()
{
	return (gSpeedRef / 10) ;
	
}

int Mascon::GetHandleValue()
{
	return gPower;
	
}

byte Mascon::GetButtonStatus(unsigned int inData)
{
	byte aRet = 0;
		
	if( (inData & BUTTON_E) > 0)
	{
		//Serial.print("SELECT,");
		aRet = aRet | JOYSTICK_SELECT;
	}
	if( (inData & BUTTON_S) > 0)
	{
		//Serial.print("START,");  
		aRet = aRet | JOYSTICK_START;
	}
	if( (inData & BUTTON_A) > 0)
	{
		//Serial.print("A,");  
		aRet = aRet | JOYSTICK_A;
	}
	if( (inData & BUTTON_B) > 0)
	{
		//Serial.print("B,");  
		aRet = aRet | JOYSTICK_B;
	}
	if( (inData & BUTTON_C) > 0)
	{
		//Serial.print("C,");  
		aRet = aRet | JOYSTICK_C;
	}
  //Serial.print("GetButtonStatus:");
  //Serial.println(aRet); 
	return aRet;
}

char* Mascon::GetHandlePosition()
{
  return gHandle_pos_str;  
}


byte Mascon::GetHandleStatus(unsigned int inData)
{
  if( (inData & MASK_BRK) == GEAR_NOT)
  {
    //不定
    //gPower はそのまま
    sprintf(gHandle_pos_str,"--");
  }
	else if( (inData & MASK_BRK) == GEAR_EMG)
	{
		//Serial.print("EMG,");  
		gPower = -1024;
    sprintf(gHandle_pos_str,"EB");
	}
  else if( (inData & MASK_BRK) == GEAR_E5)
  {
    //Serial.print("EMG,");  
    gPower = -160;
    sprintf(gHandle_pos_str,"E5");
  }
  else if( (inData & MASK_BRK) == GEAR_E4)
  {
    //Serial.print("EMG,");  
    gPower = -80;
    sprintf(gHandle_pos_str,"E4");
  }
  else if( (inData & MASK_BRK) == GEAR_E3)
  {
    //Serial.print("EMG,");  
    gPower = -40;
    sprintf(gHandle_pos_str,"E3");
  }
  else if( (inData & MASK_BRK) == GEAR_E2)
  {
    //Serial.print("EMG,");  
    gPower = -20;
    sprintf(gHandle_pos_str,"E2");
  }
  else if( (inData & MASK_BRK) == GEAR_E1)
  {
    //Serial.print("EMG,");  
    gPower = -10;
    sprintf(gHandle_pos_str,"E1");
  }
	else if ( (inData & MASK_BRK) == GEAR_B8)
	{
		//Serial.print("B8,");  
		gPower = -8;
    sprintf(gHandle_pos_str,"B8");
	}
	else if ( (inData & MASK_BRK) == GEAR_B7)
	{
		//Serial.print("B7,");  
		gPower = -7;
    sprintf(gHandle_pos_str,"B7");
	}
	else if ( (inData & MASK_BRK) == GEAR_B6)
	{
		//Serial.print("B6,");  
		gPower = -6;
    sprintf(gHandle_pos_str,"B6");
	}
	else if ( (inData & MASK_BRK) == GEAR_B5)
	{
		//Serial.print("B5,");  
		gPower = -5;
    sprintf(gHandle_pos_str,"B5");
	}
	else if ( (inData & MASK_BRK) == GEAR_B4)
	{
		//Serial.print("B4,");  
		gPower = -4;
    sprintf(gHandle_pos_str,"B4");
	}
	else if ( (inData & MASK_BRK) == GEAR_B3)
	{
		//Serial.print("B3,");  
		gPower = -3;
    sprintf(gHandle_pos_str,"B3");
	}
	else if ( (inData & MASK_BRK) == GEAR_B2)
	{
		//Serial.print("B2,");  
		gPower = -2;
    sprintf(gHandle_pos_str,"B2");
	}
	else if ( (inData & MASK_BRK) == GEAR_B1)
	{
		//Serial.print("B1,");  
		gPower = -1;
    sprintf(gHandle_pos_str,"B1");
	}
	else if ( (inData & MASK_BTN) == GEAR_N)
	{
		//Serial.print("N,");  
		gPower = 0;
    sprintf(gHandle_pos_str," N");
	}
	else if ( (inData & MASK_ACC) == GEAR_P1)
	{
		//Serial.print("P1,");  
		gPower = 1;
    sprintf(gHandle_pos_str,"P1");
	}
	else if ( (inData & MASK_ACC) == GEAR_P2)
	{
		//Serial.print("P2,");  
		gPower = 2;
    sprintf(gHandle_pos_str,"P2");
	}
	else if ( (inData & MASK_ACC) == GEAR_P3)
	{
		//Serial.print("P3,");  
		gPower = 3;
    sprintf(gHandle_pos_str,"P3");
	}
	else if ( (inData & MASK_ACC) == GEAR_P4)
	{
		//Serial.print("P4,");  
		gPower = 4;
    sprintf(gHandle_pos_str,"P4");
	}
	else if ( (inData & MASK_ACC) == GEAR_P5)
	{
		//Serial.print("P5,");  
		gPower = 5;
    sprintf(gHandle_pos_str,"P5");
	}		
}


void Mascon::PutButtonBuffer(unsigned int inData)
{
	gPsxData[2] = gPsxData[1];
	gPsxData[1] = gPsxData[0];
	gPsxData[0] = inData;
}

byte Mascon::CheckButtonBuffer(unsigned int inData)
{
	byte aOk = 0;
	
	for( int i = 0; i < 3; i++)
	{
		if( gPsxData[i] == inData)
		{
			aOk++;
		}
	}
    //Serial.print("CheckButtonBuffer:");
    //Serial.println(( (aOk >= 2) ? 1 : 0));
	
	return (aOk >= 2) ? 1 : 0;
}

