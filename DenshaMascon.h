/*  PSX Controller Decoder Library (DenshaMascon.h)
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

#include <Arduino.h>


// Button Hex Representations:
#define psxLeft		0x0001 
#define psxDown		0x0002
#define psxRight	0x0004
#define psxUp		0x0008
#define psxStrt		0x0010
#define psxSlct		0x0080

#define psxSqu		0x0100
#define psxX		0x0200
#define psxO		0x0400
#define psxTri		0x0800
#define psxR1		0x1000
#define psxL1		0x2000
#define psxR2		0x4000
#define psxL2		0x8000

//#define MASCON_dataPin 6
//#define MASCON_cmndPin 7
//#define MASCON_attPin 9
//#define MASCON_clockPin 8
#define MASCON_dataPin A3
#define MASCON_cmndPin A2
#define MASCON_attPin A1
#define MASCON_clockPin A0


#define BUTTON_E 0b0000000010000000
#define BUTTON_S 0b0000000000010000
#define BUTTON_A 0b0000000100000000
#define BUTTON_B 0b0000001000000000
#define BUTTON_C 0b0000010000000000
#define MASK_BTN 0b1111100000001111

#define MASK_BRK 0b1111000000000000
#define MASK_ACC 0b0000100000001111

//#define ACC_FLAG 0b1101000000001010
//#define DEC_FLAG 0b1111000000001111

//不定
#define GEAR_NOT 0b1111000000000000

//EMG
#define GEAR_EMG 0b0000000000000000
#define GEAR_E5 0b0010000000000000
#define GEAR_E4 0b1000000000000000
#define GEAR_E3 0b1010000000000000
#define GEAR_E2 0b0001000000000000
#define GEAR_E1 0b0011000000000000

//通常ブレーキ
#define GEAR_B8 0b1001000000000000
#define GEAR_B7 0b1011000000000000
#define GEAR_B5 0b0110000000000000
#define GEAR_B6 0b0100000000000000
#define GEAR_B2 0b0101000000000000
#define GEAR_B4 0b1100000000000000
#define GEAR_B3 0b1110000000000000
#define GEAR_B1 0b0111000000000000
#define GEAR_N  0b1101000000001111

//加速
#define GEAR_P1 0b0000100000001110
#define GEAR_P2 0b0000000000001110
#define GEAR_P3 0b0000100000001011
#define GEAR_P4 0b0000000000001011
#define GEAR_P5 0b0000100000001010


/*
#define GEAR_EMG 0b0000000000001111
#define GEAR_B8 0b1001000000001111
#define GEAR_B7 0b1011000000001111
#define GEAR_B5 0b0110000000001111
#define GEAR_B6 0b0100000000001111
#define GEAR_B2 0b0101000000001111
#define GEAR_B4 0b1100000000001111
#define GEAR_B3 0b1110000000001111
#define GEAR_B1 0b0111000000001111
#define GEAR_N  0b1101000000001111
#define GEAR_P1 0b1101100000001110
#define GEAR_P2 0b1101000000001110
#define GEAR_P3 0b1101100000001011
#define GEAR_P4 0b1101000000001011
#define GEAR_P5 0b1101100000001010
*/

#define JOYSTICK_SELECT 0b00001
#define JOYSTICK_START 0b00010
#define JOYSTICK_A 0b00100
#define JOYSTICK_B 0b01000
#define JOYSTICK_C 0b10000

#define SIZE_BTNBUFFER	3

#define GEAR_MODE_ACC 0
#define GEAR_MODE_SPD 1

class Mascon
{
	private:
		byte shift(byte _dataOut);

		byte _dataPin;
		byte _cmndPin;
		byte _attPin;
		byte _clockPin;
		
		byte _delay;
		byte _i;
		boolean _temp;
		byte _dataIn;
		
		byte _data1;
		byte _data2;
		unsigned int _dataOut;
		unsigned int data = 0;
		int gPower = 0;
		int gSpeedRef = 0;
		unsigned int gPsxData[SIZE_BTNBUFFER] = {0, 0, 0};
    char gHandle_pos_str[5] = "  ";


    unsigned int gDecData[8] = {1750,1500,1250,1000,750,500,250,0};
    unsigned int gAccData[5] = {400,800,1200,1600,2047}; 
    


	public:
		void setupPins(byte , byte , byte , byte , byte );		// (Data Pin #, CMND Pin #, ATT Pin #, CLK Pin #, Delay)
															// Delay is how long the clock goes without changing state
															// in Microseconds. It can be lowered to increase response,
															// but if it is too low it may cause glitches and have some
															// keys spill over with false-positives. A regular PSX controller
															// works fine at 50 uSeconds.
															
		unsigned int read();								// Returns the status of the button presses in an unsignd int.
															// The value returned corresponds to each key as defined above.
	
		byte Interval(int mode,int acc_rate,int dec_rate,int coast_rate);
		byte GetButtonStatus(unsigned int inData);
		byte GetHandleStatus(unsigned int inData);
		word GetSpeedValue();
		int GetHandleValue();
		byte CheckButtonBuffer(unsigned int inData);
		void PutButtonBuffer(unsigned int inData);

    char* GetHandlePosition();
};

