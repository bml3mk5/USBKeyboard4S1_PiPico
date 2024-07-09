# USBキーボード for 日立 MB-S1  ラズパイPicoバージョン

## フラッシュメモリへのプログラム方法

  ボードの電源を切ってから、ボードの電源投入中に BOOTSEL ボタンを押します
 (例: USB の接続中に BOOTSEL を押したままにします)。 するとピコはUSB大容量
 ストレージデバイスとして表示されます。 特殊な「.uf2」ファイルをディスクに
 ドラッグすると、このファイルがフラッシュに書き込まれ、ピコを再起動します。

 uf2ファイルは「build/Release」フォルダにあります。

 CAPS LEDが点灯すれば成功です。
 USBをPCから切り離してUSBキーボードを接続してください。

-----
# USBKeyboard for HITACHI MB-S1  Raspberry Pi Pico Version

## Programming the flash

  Depower the board, then hold the BOOTSEL button down during board power-up
 (e.g. hold BOOTSEL down while connecting the USB). The Pico will then
 appear as a USB Mass Storage Device. Dragging a special '.uf2' file onto
 the disk will write this file to the Flash and restart the Pico.

  There is the uf2 file in 'build/Release' folder.

  It is a success if the CAPS LED lights up.
  Disconnect the USB from the PC and connect a USB keyboard.

-----
 MailTo: Sasaji (sasaji@s-sasaji.ddo.jp)
 * My Webpage: http://s-sasaji.ddo.jp/bml3mk5/
 * GitHub:     https://github.com/bml3mk5/USBKeyboard4S1_PiPico
 * X(Twitter): https://x.com/bml3mk5
