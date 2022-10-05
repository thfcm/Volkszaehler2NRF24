# Volkszaehler2NRF24
A Battery powered Electric-Meter reader that sends data using nRF24 wireless.


I started this project because I want to measure my power consumption during the day and see how my tiny 300pW Solar Power Plant fits in.
Fortunately, the electric-meter is a digital one with an IR interface for consumer-use that sends out a bunch or information like the counter reading or the current power reading.
As I live for rent and there is no power plug nearby my electric-meter, I was forced to build something that can last for a long time on a Battery.

The Receiver is in my cellar room next to the room with the power meters where my tiny home-server is running.
The far-end of this power-meter reader is directly connected to the home server via USB and sends the received data via a serial interface, so you can access it using Node-Red or anything else.

## Schematic:

![image](https://user-images.githubusercontent.com/112399896/193898906-ffe60fd0-3d1a-467f-92f6-e11d3d0ee964.png)


## Hardware needed

I build my prototype using hardware that I had lying around: 
- an IR diode that matches the wavelength of the NMH smart-meter by chance (needs to be determined but most IR diodes should work)
- an Arduino Board using a 328p microcontroller. We are going to run that one consuming as few power as possible. It does not matter if you are using the 16MHz/5V or the 8MHz/3.3V Version as we are going to modify that board to waste as few power as possible. In my case I was modifying an *Pro Micro board (16MHz/5V)* to run on 4MHz and 3.3V using an external power regulator.
- an nRF24 Transceiver. I was using an nRF24L01+ as I planned to use the 250kBaud transfer rate in the hope to use even less power with that. You should do fine with an nRF24L01 (without the "+"), too. In that case, you'll need to modify some settings as the non-plus transceivers do not support 250kBaud (they 'only' do 1MBaud or 2MBaud).
Your Receiver should match the capabilities.
- A Low-Dropout power regulator that has a small power consumption by its own. I am using HT7333 , but you can use any other regulatorthat fits, too. It even might be good to only use 3.0Volt or less (as long as it fits the needs of the 328p@4MHz and the nRF24). You can also stick with the one that is integrated on your board as long as it is a 3.3V regualtor as the nRF24-board will be destroyed if you run ith with 5V.
- a Sincle Cell (1S) LiPo Battery or something similiar. Make sure you use a Battery with a protection cirquit as I didn't integrated any protection. I am using a 18650 Battery with 2600mAh capacity and an integrated protection cirquit. Make sure the final discharging voltage is high enough so your voltage regulator can provide the minimum volate to your board as long as possible. You may even get rid of the power regulator if you can make sure that the end-of-charge voltage does not exceed the maximum rating of your nRF24 module.

## Hardware modification
I removed the power LED as well as the power regulator that was mounted on the board. We do not need the LED. It only consumes power.
I also removed the integrated power regulator as it is not the most efficient one. It was replaced by the already mentioned external HT7333. You may do fine to keep the integrated one, depending on how less power you really want to waste.
I stabilized the 3.3V rail with a 220uF/10V condensator. That is optional but may help in case the nRF Module induces some peaks.


## Low Power
When running something on battery, you usually want it to run as long as possible without replacing the battery.
I had the goal to have the reader running at least 6 month and ended up with more than 12 month (in theory - that needs to be confirmed).

To do so, I chose an Arduino compatible board that does not have a lot external peripheral attached. The Pro Mini I am using does not even have an USB port as the common USB-to-Serial chips use a lot of power even if you don't need them.
I also removed the power-LED as well as the integraded power regulator.

To save even more power, I have the Atmega328p running at 4MHz and using 3.3V. The less volts, the less power it is consuming. You can also run it with 3.0 Volts or even less to save even more power. 
I disabled all the internal peripheral devices the Atmega has and that we do not need (like ADC, Brown.out.Detection and so on).

I got that information from [http://www.gammon.com.au/power](http://www.gammon.com.au/power) so all the credits for that goes there.

In addition to that, the nRF24L01+ is set to a sleep mode as long as we don't have anything to send.
Also the Atmega328p is sleeping for most of its time. At the moment, it will sleep for 16 Seconds, wakes up, waits for some Serial Data from the IR port and extracts the data we want. It then sends those 8 Bytes and goes back to sleep again immediantly.


## Used external Libraries
For that project we are using the following libraries:

* [RF24 Library from https://nrf24.github.io/RF24/](https://nrf24.github.io/RF24/)

* [AltSoftSerial from https://www.pjrc.com/teensy/td_libs_AltSoftSerial.html](https://www.pjrc.com/teensy/td_libs_AltSoftSerial.html) as the regular SoftwareSerial was too slow

* [SML Parser from https://github.com/olliiiver/sml_parser](https://github.com/olliiiver/sml_parser) to parse the SML Data the electric-meter sends


