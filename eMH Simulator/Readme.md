## eMH Simulator

This little piece of code will simulate an eMH energy-meter and will send simulated data using an IR Diode.
This way you can test your Volkszaehler2NRF24 on your desk until everything works.
<br/>
This sketch uses a binary file from the [libsml library from https://github.com/olliiiver/sml_parser](https://github.com/olliiiver/sml_parser)
<br/>                        
I used a Teensy 3.1 Board that I had laying around but any other board should work fine, too.
<br/>                                 
Simply solder an IR TX Diode between 3.3V and your TX Pin. Depending on the Diode you are using, you should install the matching current limiting resistor.

                                         
                                         
Example:<br/>
<img src="https://user-images.githubusercontent.com/112399896/194393036-e4436854-5b4e-41f3-9c04-d4acf8031f73.jpg" data-canonical-src="https://user-images.githubusercontent.com/112399896/194393036-e4436854-5b4e-41f3-9c04-d4acf8031f73.jpg" height="200" />
<br/><br/>
You can check if the Simulator is sending data using your Smartphones Camera. <br/>
Most of them are capable of "seeing" infrared light (wich seems to be a purple pale light):<br/>
<img src="https://user-images.githubusercontent.com/112399896/194393226-17a700dd-0683-4975-bb78-633fa77d8019.jpg" data-canonical-src="https://user-images.githubusercontent.com/112399896/194393226-17a700dd-0683-4975-bb78-633fa77d8019.jpg" height="200" />