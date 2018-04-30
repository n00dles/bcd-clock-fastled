# bcd-clock-fastled
BCD clock using an esp8266 and ws2812 RGB leds or any other supported led via fastled library

## Led Layout

leds are numbered 0-19 and should be arranged as follows 

![Led Layout](https://github.com/n00dles/bcd-clock-fastled/blob/master/images/layout.jpg)

On my board pictured I have a raw 5v out which I use to power the leds. 

pin 5 should be connected to the data in on Led 00. 

## First Boot 

On first boot the device will start in AP mode as 'BCDClock with a password of 'password' 

Connect to this LAN and browse to http://192.168.4.1 

Configure and connect to your desired Wifi network. Once connected the device will get the current time from the NTP server and set the clock in motion. 
