# YMF825 Legacy Midi module

Arduino MIDIシールドと YMF825boardを使用したレガシーなMIDI FM音源モジュール

https://www.switch-science.com/catalog/3399/

http://akizukidenshi.com/catalog/g/gM-12414/

https://www.switch-science.com/catalog/2492/


オムニモードで動作します。MIDIチャンネルは1-16chいずれでも発音します。
起動時にテスト音 ドミソド　が鳴りますのでボリュームにご注意下さい。
最大16ボイス発音のはず。。。

MIDIステータスは KEYON/OFF PROGCHANGE を実装しました。
音色はとりあえず４音色入っています。

MIDIシールドまたは互換ハードウェアを前提にしていますが、setup()内で　Serial.bebin(38400) 等すれば
MIDIシールド無しでパソコンのUSB COMポートとしてパソコンからも直接にコントロールできると思います。

