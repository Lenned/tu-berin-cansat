
# TU Berlin CanSat Project
During the the summer of 2019 I studied abroad in Berlin at the Berlin Institute of Technology. I took a long course entitled "CanSat: Hands-on Satellite Design". My team, consisting of 4 members, built a satellite in 4 weeks and successfully launched it on a suborbital trajectory. My role was the lead developer: I developed the onboard and ground station software, tested it, and managed the launch from the ground station.

> "A CanSat is a simulation of a real satellite, integrated within the volume and shape of a soft drink can. The challenge for the students is to fit all the major subsystems found in a satellite, such as power, sensors and a communication system, into this minimal volume." - [ESA](https://www.esa.int/Education/CanSat/What_is_a_CanSat)

## Onboard Flight Software
Our Satellite utilized an Arduino Nano as it's microcontroller. As such, the OFS was written in the Arduino language (a subset of C++). We designed the OFS with several core functions in mind:

 - Maintain communication with the ground station
	* Data is transmitted from the CanSat to the ground station at a rate of at least 10hz
	* Several different packet types may be sent back and forth, including data packets containing calibrated sensor data, status packets containing datapoint about the status of satellite subsystems, and command packets containing actions
	* We utilized XBees for uplink/downlink
 - Receive raw data from the sensors and calibrate/process it before transmitting to the ground
	* The satellite's sensors are calibrated on the launchpad
	* The OFS performs calculations on the sensor data such as slope and ensures the data is accurate and within expected parameters
 - Automatically manage the satellite subsystems during each phase of flight
	* We identified several phases of flight that our satellite will undergo:
		* Pre-Flight
		* Ascending
		* Descending
		* Landing
		* Post-Landing
	* The OFS automatically keeps track of the current flight phase, performs the required functions of that state, and manages the transition to the next flight phase
	* We tested the satellite in a wide variety of conditions, collecting a plethora of sensor data. We used this data to determine the data parameters required each flight phase transition to occur.
 - Allow manual control from the ground station for all functions of the satellite
	* The ground station can uplink commands to the satellite
	* Commands can be as broad as "Switch to the descending flight phase" or as specific as "Move the parachute release servo to 35 degrees"
	* The ground station can send one-off commands or completely disable the automatic control system and fully switch to manual control of the satellite
 - Successfully release the parachute and drive 5m once landed
	* We identified this requirement as the most troublesome: An early parachute deployment or enabling of the driving motors during fight would be catestrophic
	* A number of safety systems were put into place in order to ensure the correct timing of the parachute deployment and post-landing driving
	* When automatically deciding whether to deploy the parachute or not, the OFS takes into consideration not only the sensor data but also the timing of the launch phases and any objections from the ground station

We successfully integrated all of the above functions into the completed OFS.

## Ground Station Software
We utilized [NASA openMCT](https://github.com/nasa/openmct) for our ground station software. However, we had to develop a backend server to connect the incoming data from the XBee to openMCT. In addition, we had to develop the uplink server and frontend from scratch as openMCT only supports displaying downlink data. We also customized openMCT for our CanSat and designed a mission control layout in order to allow ground controllers to efficiently manage the satellite. Finally, we linked all of the ground station servers (uplink, downlink, historical, openMCT) so that they automatically start up together as a single package.

Our backend and uplink servers are written in Javascript using Node.JS and utilize a number of dependancies: notably [openMCT](https://github.com/nasa/openmct), [Express](https://expressjs.com), [serialport](https://www.npmjs.com/package/serialport), and [xbee-api](https://www.npmjs.com/package/xbee-api).

## Project Outcomes

## Credits
### Project Members:

 - Matthew Brand
 - Ben Thomson
 - Carmen Muzquiz
 - Ronald Pillinger
