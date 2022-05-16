# Orangeboard
The goal of this project is to create a device that digitizes writing on a whiteboard in real-time.

# Overview
IR Reflective tape is applied to a whiteboard marker. A device is applied to each corner of the whiteboard. Because the purpose of these devices is to chart the marker as it moves along the white board, I have designated these 4 nodes as "scouts". The 4 scouts communicate with a main node which I refer to as the "captain".
![20220516_150625](https://user-images.githubusercontent.com/90781321/168684107-fae5b3ba-30a8-41e3-acc1-6d2115eaf488.jpg)



# Scout
Each scout consists of two circuits, each powered by a 9 volt battery.
![20220516_145415](https://user-images.githubusercontent.com/90781321/168684414-6fb3750b-c58d-4fc3-a666-fc2e9ae29ac3.jpg)
![20220516_072759](https://user-images.githubusercontent.com/90781321/168684525-83288816-d222-4534-8c05-7559b22013c8.jpg)
![20220516_145447](https://user-images.githubusercontent.com/90781321/168684367-17d4f64f-75e0-4a6f-88f9-4ab8f0e55b34.jpg)
![20220516_145442](https://user-images.githubusercontent.com/90781321/168684388-3db38cf1-d481-4f0a-bcbe-572b4e280b2d.jpg)


The main circuit consists of 4 main components:
1. Arduino Nano
2. 4 IR Receivers
3. 1 IR Communication Receiver
4. 1 433MHz Radio Transmitter

![20220516_152108](https://user-images.githubusercontent.com/90781321/168684692-03326296-41d2-4e42-8b12-6359d9db6340.jpg)
![20220516_150727](https://user-images.githubusercontent.com/90781321/168684856-28e164aa-dbbf-4cea-a803-66df8609720d.jpg)

The second circuit simply powers 5 IR emmiters.


![20220516_082744](https://user-images.githubusercontent.com/90781321/168685090-4d4c9673-7224-4c8a-a969-1c5a5bb6dbba.jpg)


The casing for each of the scouts was 3D printed with generic PLA using an Anet A8 and an Ender 3. It consists of 3 parts. The frame, the battery case, and the lid.  The stl files are included in the folder labeled "Scout Prints".

# Captain
The captain consists of an Arduino Nano and a Raspberry Pi Zero. Attached to the nano is a 433Mhz radio receiver as well as an IR emitter. The IR emitter draws more current than the Nano can supply, so it is powered externally (I have been using a power supply, but I intend to make it so that it plugs into the wall) and controlled with an npn transistor.

![20220516_151345](https://user-images.githubusercontent.com/90781321/168684248-dfc6c001-b953-4ec3-b2be-6d383097a03f.jpg)

# Networking
This project relies on 5 nodes interacting dynamically with each other, so networking is essential to the project. The protocol I came up with had to solve several problems:
1. An arduino nano can either have a transmitter or receiver attached, it cannot have both (limitation due to modules I chose).
2. A receiver can only receive one message at a time (physical limitation of single wire).
3. Messages do not always arrive, so will need to be retransmitted selectively (limitation of cheap radio modules).
4. Messages need to correspond to a specific time (since they will be out of order).

## Physical Layer
I chose to use radio for communication from the scouts to the captain, and infrared communication from the captain to the scouts. This solved limitation #1. 
I had range issues originally for both the radio modules and the IR system. I solved the range issue for the radio modules by attaching a 1/4 wavelength antenna to both the transmitter and receivers. wavelength/4 = (c/4f) = (3x10^8 m/s)/(4x433x10^6Hz) = 17.32cm. A more powerful IR emitter and separate power supply solved the range issue for the IR system.

## Link Layer
I used the arduino <a href="https://www.airspayce.com/mikem/arduino/RadioHead/">RadioHead</a> library for radio communication, and the <a href="github.com/Arduino-IRremote/Arduino-IRremote">IRremote</a> library for IR communication. The IR library uses the same protocols as a for a tv remote, so address can be read intuitively as separate from a command. This is useful since we need both the address of the scout as well as the command (send, or missing packet).

I made use of the examples for these libraries to create my code.

## Data
The Data is sent from the scouts to the captain in the following format:

<ID, Packet, Readings, Time> 
This is read as a string and separated with key characters. Here is an example:
"n1i1233w1022x1022y1016z1021t2780"

| Character|   Sequence    |
| ---------| ------------- |
|     n    |      ID       |
|     i    | packet Number |
|  w,x,y,z | values of 4IR |
|     t    | time (in ms)  |

      
Note the time is the clock on the arduino right after the IR values were read. This almost solves problem 4, but not quite since it relative to when the arduino for that specific scout was turned on.

To solve problem 2, the captain sends an IR signal containing an address, so the correct scout will respond. The command is to either send all data in the scouts buffer, or to send a specific packet.

The captain simply rotates through each scout, getting the data from each buffer, then sends it to the Raspberry pi. Before it moves on to the next scout, the captain makes sure there are no packets missing, and keeps requesting the missing packet (via the IR emitter) until that packet is received (solving problem 3).

The scouts collect the IR data in a buffer until they receive a command to transmit everything. They then transmit everything (via radio). They then only send repeat information when it is requested.



# Status
All 4 scouts are complete (after around 30 hours of soldering). A better package for the captain should be constructed. Currently, the scouts can recognize commands, and can transmit the data in the buffer. The Captain can send commands and receive the data, but it is difficult to discern what control features work, because results are inconsistant. This is probably due to the fact the memory is nearly maxed out.

An intermittent timing system needs to be added so that the various times can be adjusted (to completely solve problem 4). This would work by the captain simply requesting the time over IR, and comparing it to its clock. An adjustment vector could then be sent to the pi separately.

Raspbery Pi: The program to read data from the nano is functioning, as is a program to move data over the internet. These still need to be combined.

Once the data is a received, an application needs to be made on the receiving computer to process the data, to make sense of the location of the pen.



