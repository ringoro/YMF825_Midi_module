# YMF825 Legacy Midi module

2017/10/3 ピッチベンド関数更新

Arduino MIDIシールドと YMF825boardを使用したレガシーなMIDI FM音源モジュール

https://www.switch-science.com/catalog/3399/

http://akizukidenshi.com/catalog/g/gM-12414/

https://www.switch-science.com/catalog/2492/


オムニモードで動作します。MIDIチャンネルは1-16chいずれでも発音します。
起動時にテスト音 ドミソド　が鳴りますのでボリュームにご注意下さい。
最大16ボイス発音のはず。。。

MIDIステータスは KEYON/OFF ベロシティ対応　、PROGCHANGE

CC 1 Modulation

CC 7 Master Vol.

CC 123 All Note Off

Pichbend 

対応しました。

音色はとりあえず４音色入っています。

MIDIシールドまたは互換ハードウェアを前提にしていますが、setup()内で　Serial.bebin(38400) 等すれば
MIDIシールド無しでパソコンのUSB COMポートとしてパソコンからも直接にコントロールできると思います。


# debug

2017/10/7 ピッチベンドテーブル値　yamahaサンプルでは最大値は1023になっていますが、これではオクターブ上が1.68セント低くなるので不自然に聴こえるかもしれません。その場合は1024を設定した方が良い様です。<br>
2017/10/3 ピッチベンド関数をテーブル方式に変更 <br>
参考URL  :  https://github.com/yamaha-webmusic/ymf825board/tree/master/sample2

2017/9/29 ピッチベンド関数修正しました

2017/11/15 midiボーレートがミスしております　31500 boud になっていますが正しくは 31250 です



