# Volkszaehler2NRF24
A Battery powered Power-Meter reader that sends its Data using nRF24 wireless.


I started this project because I want to measure my power consumption during the day and see how my tiny 300pW Solar Power Plant fits in.
As I live for rent and there is no power plug nearby my power meter, I was forced to build something that can last for a long time on a Byttery.

The Receiver is in my cellar room next to the room with the power meters where my tiny home-server is running.
The far-end of this power-meter reader is directly connected to the home server via USB and sends the received data via a serial interface, so you can access it using Node-Red or anything else.

## Schematic:

![image](https://user-images.githubusercontent.com/112399896/193898906-ffe60fd0-3d1a-467f-92f6-e11d3d0ee964.png)


## Hardware needed

I build my prototype using hardware that I had lying around: 
- an IR diode that matched the wavelength of the NMH smart-meter by chance (needs to be determined)
- an Arduino Board using a 328p microcontroller. We are going to run that one using as few power as possible. It does not matter if you are using the 16MHz/5V or the 8MHz/3.3V Version as we are going to modify that board to waste as few power as possible. In my case I was modifying an pro  micro board (16MHz/5V) to run on 4MHz and 3.3V using an external power regulator.
- an nRF24 Transceiver. I was using an nRF24L01+ as I wanted to use the 250kBaud transver rate in the hope to use even less power with that. You should do fine with an nRF24L01 (without the "+"), too. In that case, you'll need to modify some settings as the non-plus transceivers do not support 250kBaud (they 'only' do 1MBaud or 2MBaud).
Your Receiver should match the capabilities.
- A Low-Dropout power regulator that has a small power consumption by its own. I am using HT7222 , but you can use any other regulatorthat fits, too. It even might be good to only use 3.0Volt or less (as long as it fits the needs of the 328p@4MHz and the nRF24). You can also stick with the one that is integrated on your board as long as it is a 3.3V regualtor as the nRF24-board will be destroyed if you run ith with 5V.
- a Sincle Cell (1S) LiPo Battery or something similiar. Make sure you use a Battery with a protection cirquit as I didn't integrated any protection. I am using a 18650 Battery with 2600mAh capacity and an integrated protection cirquit. Make sure the final discharging voltage is high enough so your voltage regulator can provide the minimum volate to your board as long as possible. You may even get rid of the power regulatorif you can make sure that the finale charge volate does not exceed the maximum rating of your nRF24 module.

## Hardware modification
I removed the power LED as well as the power regulator that was mounted on the board. We do not need the LED. It will only consume power that we don't want to waste.
I also removed the integrated power regulator as it is not the most efficient one. You may do fine to keep the integrated one, depending on how less power you really want to waste.
I stabilized the 3.-3V rail with a 220uF/10V condensator. That is optional and you might also be able to run the transceiver without that. I integrated that cpacitor in oorder to stabilize the regulated 3.3V rail in case the transceiver generates some power-peaks.









