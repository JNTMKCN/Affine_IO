Affine IO

本IO使用与官方串口协议相似的协议连接手台，但是在协议内添加了一些自定义命令，让同一个串口实现JVS板功能，如IR KEY，灯光，以及未来可能支持的Coin Key。
添加对游戏内灯光的输出支持
在segatools.ini中使用：
[led15093]
; Enable emulation of the 15093-06 controlled lights, which handle the air tower \
; RGBs and the rear LED panel (billboard) on the cabinet.\
enable=1

`[led]
; Output billboard LED strip data to a named pipe called "\\.\pipe\chuni_led"
cabLedOutputPipe=0
; Output billboard LED strip data to serial
cabLedOutputSerial=1`

`; Output slider LED data to the named pipe
controllerLedOutputPipe=0
; Output slider LED data to the serial port
controllerLedOutputSerial=0`

`; Serial port to send data to if using serial output. Default is COM5.
serialPort=COM8
; Baud rate for serial data
serialBaud=115200`

`; Data output a sequence of bytes, with JVS-like framing.
; Each "packet" starts with 0xE0 as a sync. To avoid E0 appearing elsewhere,
; 0xD0 is used as an escape character -- if you receive D0 in the output, ignore
; it and use the next sent byte plus one instead.
;
; After the sync is one byte for the board number that was updated, followed by
; the red, green and blue values for each LED.
;
; Board 0 has 53 LEDs:
;   [0]-[49]: snakes through left half of billboard (first column starts at top)
;   [50]-[52]: left side partition LEDs
;
; Board 1 has 63 LEDs:
;   [0]-[59]: right half of billboard (first column starts at bottom)
;   [60]-[62]: right side partition LEDs
;
; Board 2 is the slider and has 31 LEDs:
;   [0]-[31]: slider LEDs right to left BRG, alternating between keys and dividers`

编译DLL文件：

`gcc -shared -o affine.dll chuniio.c config.c dprintf.c ledoutput.c pipeimpl.c serialimpl.c serialslider.c`

在Segatool中使用：

`[chuniio]
; If you wish to sideload a different chuniio, specify the DLL path here`
`path32=affine_x86.dll`
`path64=affine_x64.dll`


