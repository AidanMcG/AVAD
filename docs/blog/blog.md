# Blog: Arduino vehicle anti-theft device

**Aidan McGivern**

## My First Blog Entry 24/10/18

Stephen Blott is going to supervise my project. I have completed my project 
proposal which I will be presenting today to gain approval for it. 
If i get approval I will start working on acquiring the required arduino
 bits necessary to complete my project.

## Second Blog Entry 02/11/18

I have been doing research to find what specific arduino modules I will need. 
This is one of my bigger challenges as I need to make sure that the parts
I buy will work together and will also achieve a good end result.

## Third Blog Entry 08/11/18

I am now starting to get going on my project and have everything ready to get 
working. I have started writing my functional spec and I have also started 
to order the relevant arduino parts that I need. I have started to use the 
gitlab from the command line and have my computer setup to run natively on 
linux to ensure an easy enviroment to code in. I am currently bringing 
myself back up to speed on git commands, which I am testing as I am writing 
this blog.

## Fourth Blog Entry 10/11/18

I have now completed a full outline of my functional spec. From this I have
 completed about a third of the final document. I have familiarized myself
 with the git command line and I am now using it as my main source of 
interaction with gitlab. I hope to have my functional spec completed 
a couple of days before the deadline to give myself some breathing space.

## Fifth Blog Entry 28/11/18

Today I met with Stephen Blott to show him my Functional Spec. He was very
happy with it and had very little negative to say about it. The one thing
that he did suggest, was to add another option of being able to recover 
the users pin code by texting help to the AVAD. I am going to add this to 
my functional spec. After that, I will only have to make a few small edits
to the rest of the document then I will submit it.

In other news I have discovered that I will be able to send the user a link
that will open the current location of the car in the google maps app. This
will make it easier for the user to find the car and make it faster than 
copying and pasting coordinates.

## Sixth Blog Entry 03/12/18

I have submitted my functional spec on Friday on time. I am now going to 
concentrate on getting all of the hardware together and building a prototype.

## Seventh Blog Entry 29/01/19

After taking time out of my project to study for my exams I have now ordered 
the last of the hardware. It should arrive by the end of the week. I have 
already done a lot of research into the parts and should be able to get going
on the project very quickly.

## 03/02/19

All of the parts have now arrived and I have begun to piece the hardware
together. I need to get a soldering iron so that I can solder headers onto the 
gps module.

## 05/02/19

I now have the headers soldered onto the gps board. The arduino is connected to 
the bluetooth module, the gps module and the OBD2 module via breadboard and 
are all powering on correctly. It turns out that I ordered an incorrect part. 
I ordered the actual chip for a GSM module rather than the actual module. I 
also need to order a GPS aerial and an SMA to FMA adapter in order to repurpose 
and old GSM antenna from a nokia car kit.

## 08/02/19

Ordering a GSM module is proving more difficult than initially anticipated. I 
am putting more research in this time before placing an order. In the mean time 
I have begun to communicate via Serial to the bluetooth module using the arduino
and can connect to the device from my Mac.

## 12/02/19

I have now managed to pull a GPS location from the GPS module. It transmits 
a GGA message with coordinate, time and other pieces of information. The 
are slightly off as it has my location on a green two roads away but I am sure 
that the addition of an antenna will increase the accuracy of the location.

## 15/02/19

I have now ordered what I hope will be the last parts of the project. While 
waiting for the parts to arrive I have started to try and use the bluetooth and
GPS modules to work together correctly.

## 18/02/19

I have been tinkering with the bluetooth and GPS modules and I am finding it 
difficult to get them to do what I want them to do but I hope I will get used 
to the arduino system.

## 21/02/19

The final parts of my project have arrived. I now have to solder headers onto 
the GSM module and connect all of the antennas to the deivces.

## 22/02/19

The antenna for the GPS module has drastically improved the location of the 
coordinates. I can now accurately pinpoint my location even inside.

## 27/02/19

Currently I am trying to communicate with the GSM module. I am running into an 
error which I am struggling with. For some reason the arduino when connected to 
the GSM module produces the error "Could not connect to FONA". I am trying to 
diagnose why this is happening and hopefully after this I will have all of the 
components of the project working. At the same time I have figured out a way to 
take the NMEA GPS messages, convert them into co-ordinates and then format them 
into a link for Google Maps. For example today in the labs I produced this
location: https://www.google.ie/maps/search/53.3857133,-6.257801667

## 11/03/19

After a long struggle I have finally figured out that the breadboard I had the 
GSM module connected to was the issue. I have now managed to get the GSM 
module to start communicating with the arduino by moving all of the components 
onto the one breadboard. My next problem is to get the sim card communicating 
with the network and then I can finally begin to develop my code properly. I 
have contacted vodafone to ensure that the sim card is a 2G compatible sim card, 
which it is. For some reason I cannot get network reception with the sim card. 
I have done some research and there is a possibility that the issue is with the 
battery so I am going to replace it and see if that fixes the problem. If that 
does not fix the problem I am going to have to replace the antenna.

## 25/03/19

Having struggled with this for a long time I have finally fixed the issue I was 
having with the GSM module. I replaced the battery and installed a new antenna 
to no avail. I then set about using different code to try and make it work 
rather than the stock test code. After managing to make it work with different 
code and a different sim card I have finally managed to get the hardware 
working. I will now be able to begin to code my project properly.

## 28/03/19

After getting the sim card module to start to function correctly, it all of a 
sudden has decided not to work and I am now working on trying to resurrect the 
module and see if I can get it to work reliably.

## 4/03/19

Having made no progress on fixing the sim card module I have decided to leave it 
to one side while I focus on the rest of the project. I have decided to start to 
progress the bluetooth module.

## 8/03/19

After succeeding in changing settings on the bluetooth module to suit my needs, 
I rebooted the device and it has now died. I have no idea why it has died as all 
I did was reboot it. I am now going to have to replace the bluetooth module and 
the sim card module.

## 12/03/19

I have changed the way that the GPS gets its coordinates. Instead of using a 
GPGGS and  GPRMC NMEA message I now only use a GPRMC message which allows me to 
gather the coordinates and speed. I have just come back from using the GPS while 
driving my car with it on and it provided accurate GPS location and accurate 
speed. The speed is given in knots but can be easily converted to kilometers per 
hour which I have done.

## 16/03/19

I have just placed an order for a new blueooth module and a new GSM module. I 
have ordered a different battery in the hopes that that will fix the issues that
I was having with the module.

## 22/03/19
The new modules have arrived today and I hope that they will work

## 24/03/19

After testing the new devices I have broken the new bluetooth module. I have
realised that the voltage on the technical sheet is in fact wrong. The new sim 
card module however works perfectly with the new battery.

## 25/03/19

Today I have ordered a new bt module.

## 29/03/19

The new bluetooth module arrived today and is starting to work correctly. I have
now realised that in order to configure the device newline characters must be 
first disabled and then enabled after entering the configuration mode.
Otherwise it is impossible to configure the device.

## 05/04/19

Having been able to get all the modules to work in a stable manner I can now 
continue to develop the code for the AVAD. I have been able to figure out how
to properly use character arrays as it is not reccommended to use Strings in 
the arduino code.

## 10/04/19

I have split the main loop into three main parts the setup, the unconnected 
bluetooth stage and the connected bluetooth stage. Each stage is navigated 
through the alteration of global variables.

## 14/04/19

Now that the sim card module is working more reliably i am able to 
programmatically iterate through all of the new messages and compare them to the 
predefined commands that can be issued through text by the user.

## 16/04/19

I have realised that the bluetooth profile used by the bluetooth module is not
quite suited to the connection of phones as an external app is required to 
change a setting on the phone to enable the device to connect to the phone. 
Nevertheless I will try and make it work.


## 18/04/19

I have decided to not use the bluetooth library as it is not useful at all. 
Instead I have created all the functions needed for the bluetooth module through
my own commands.

## 25/04/19

I have had to bring in some delay() commands as some code that interacts with 
the serial ports will not work properly unless there is an adequate delay. As a 
result I have to go through many lines of code just to make sure that they will 
work.

## 30/04/19

The code is now mostly working. I have been able to read and write all 
required settings to and from the eeprom. I still however have yet to implement 
how the AVAD will initially connect to the users phone through bluetooth.

## 06/05/19

I have added a function that enables the arduino to be reset and to reset all 
of the other modules at the same time.

## 12/05/19

I have now managed to make the AVAD scan for bluetooth devices and try to 
initiate a connection to each device found. In order to do this I had to figure 
out a way to enlarge the RX_REPLY_BUFFER for the serial port. After a bit of
research I figured out that I had to modify the code that pushed my code to the 
arduino in the files of the arduino IDE

## 18/05/19

Over the last few weeks I have started the final documents and I am currently 
finalising them.

## 19/05/19

Today I created and editted the video walkthrough for my project. I have now 
completed the documents and will be uploading them soon. I fixed a couple of
bugs that had appeared and I am now making sure that everything that I need to 
upload is uploaded.