# Mascon2Switch
## 電車でGoマスコンをスイッチに繫ぐときのスケッチ  
- <https://tangential-star.hatenablog.jp/entry/20201107/1604761008>に従って、IDE,STL,SwitchControlLibraryをインストール
- Arduino pro micro使用。ただし、ボードはLeonardoを選んでおく。
- <https://github.com/fujigaya2/PSCon2Switch/blob/main/schematic.png>のようにつなげば、動くたぶん。
- ボードがうまく書き込めていれば、windowsのデバイスマネージャで、<https://github.com/fujigaya2/PSCon2Switch/blob/main/device.jpg>のようなVID,PIDで見えるはず。
- こんなボードにして動いている。<https://seesaawiki.jp/fujigaya2/d/Mascon2kbd> 

- 電車でGoコントローラ操作は、  
1.マスコンノッチ  
2.Select  
3.Start　→　Xボタン  
4.A　→　Yボタン  
5.B　→　Bボタン  
6.C　→　Aボタン  
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
