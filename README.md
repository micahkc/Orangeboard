# Orangeboard
The goal of this project is to create a decive that digitizes writing on a whiteboard in real-time.

# Overview
IR Reflective tape is applied to a whiteboard marker. A device is applied to each corner of the whiteboard. Because the purpose of these devices is to chart the marker as it moves along the white board, I have designated these 4 nodes as "scouts". The 4 scouts communicate with a main node which I refer to as the "captain".

# Scout
Each scout consists of two circuits, each powered by a 9 volt battery.
The main circuit consists of 4 main components:
1. Arduino Nano
2. 4 IR Receivers
3. 1 IR Communication Receiver
4. 1 433MHz Radio Transmitter
Here is the schematic:

The second circuit simply powers 5 IR emmiters. The schematic:



The casing for each of the scouts was 3D printed with generic PLA using an Anet A8 and an Ender 3. It consists of 3 parts. The frame, the battery case, and the lid.  The stl files are included in the folder labeled "Scout Prints".

# Captian
The captain consists of an Arduino Nano and a Raspberry Pi Zero. Attached to the nano is a 433Mhz radio receiver as well as an IR emitter. The IR emitter draws more current than the Nano can supply, so it is powered externally and controlled with an npn transistor.

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
**Character**     **Sequence**
      n                 ID
      i           packet number
   w,x,y,z  values of each of the 4 IR
      t            time (in ms)
      
Note the time is relative to when the arduino for that specific scout was turned on.


# Status
4
