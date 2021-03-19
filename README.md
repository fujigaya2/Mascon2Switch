# Mascon2Switch
　電車でGoマスコンをスイッチに繫ぐときのスケッチ  
 ハード図面は  
https://seesaawiki.jp/fujigaya2/d/Mascon2kbd  
を流用。  
現在、作り途中。  
操作タイプはスタンダードを想定  

・arduinoSTLライブラリを入れる必要あり。  
https://www.arduinolibraries.info/libraries/arduino-stl  
・Boards.txtを一部変更する。
(Arduinoがインストールされたディレクトリ)/hardware/arduino/avr/boards.txtを開き

leonardo.vid.1=0x0f0d  
leonardo.pid.1=0x0092  
・・・  
leonardo.build.vid=0x0f0d  
leonardo.build.pid=0x0092  
と変更しておきます。（理由は知りません）  

操作は、  

電車でGoコントローラは  
１．マスコンノッチ  
２．Select  
３．Start　→　Xボタン  
４．A　→　Yボタン  
５，B　→　Bボタン  
６，C　→　Aボタン  
のノッチ＋５ボタンしかなく、おそらく、これだけだとメニュー操作が出来なさそうなので、  

Selectを押しながらの時の操作は  
上：Start  
左：Aボタン  
下：Bボタン  
右：Cボタン
マスコンブレーキ側：Rボタン
マスコン加速側：Lボタン
です。 


## 使用しているライブラリ（Copy済み）  
・Switch Control Library（V2.0.0)  
https://github.com/celclow/SwitchControlLibrary

・PSX Controller Decoder Library (DenshaMascon.cpp)  
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
