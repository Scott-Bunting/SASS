# Requirements Analysis/Specification

This document contains the requirements analysis, outlining an initial specification for the SASS.  
*Subject to change in future editions as prototyping and testing may show that other solutions are more suitable.*

## Notes
* Write about EEG and how it may be more robust than PPG.
* Make descriptions more related to actual context.
* Revise section on IMU. Currently a bit wishy washy and specific reports using accelerometer data would be valuable.
* Add photos of each sensor and potentially visualisation for PPG and blood flow.
* Update the README.md file as a result of changes made here.
* Update the HMI section with more detail, selecting one method of interfacing with the defice for the lab/bench prototype.

## Physiological Signs

### Autonomic Nervous System

**Confirmed**
* Heart-Rate
* Skin Conductance

### Behavioural

**Confirmed**
* Movement

**Possible**
* Voice

## Contextual Information

### Environment

**Confirmed**
* Movement
* Ambient Sound

**Possible**
* Temperature
* Location

**Under Consideration**
* Calendar

## Sensors

### Data Definitions

Personal Data
* Personal data refers to data which is indicative of ones emotional state, and therefore of how much SA they may be experiencing.

Contextual Data
* Contextual data refers to data which is indicative of ones environment, and consequently what may lead to the onset of SA.


### Galvanic Skin Response (GSR) - Grove GSR Sensor
*Personal Data*

#### Description
The GSR sensor directly measures skin conductance, also known as Electrodermal Activity (EDA) or Galvanic Skin Resistance (GSR).
Skin conductance is a physiological response to emotional arousal and is controlled by the Autonomic Nervous System (ANS),
meaning that one has no conscious control over their skin conductance. As individuals with Social Anxiety (SA) are significantly 
emotionally aroused, when experiencing an especially anxiety inducing event, skin conductance can be used to show this. 
Therefore the use of a GSR sensor, along with others to be mentioned, will be a good indicator of SA levels.

### Photoplethysmography (PPG) - MAX86160EFN+ PPG & Pulse-Oximetry
*Personal Data*

#### Description 
PPG sensors are small, low-cost sensors used to measure Heart-Rate (HR). They measure this by flashing a light through the skin
and then recording the amount of light reflected. When blood is pumped, by a contraction of the heart, red blood cells align with 
the direction of flow and reflect more light. This characteristic allows the HR to be measured, although this measurement
technique isn't very robust and is easily distorted by movement or sound. HR, and more specifically Heart-Rate Variability (HRV),
are indicators of one's emotional response to an environment, and as a result are strong indicators of when someone may be experiencing
an anxiety inducing event.  
PPG sensors are becoming evermore present in consumer devices. They are currently embedded in most smartwatches and smartphones such as 
the Samsung Galaxy series (S5 {2014} onwards [1]) and are used for health and wellbeing applications.

### Microphone - MAX9814 Microphone & Amplifier
*Personal/Contextual Data*

#### Description
Microphone's are small, low-cost, audio input devices which are ubiquitious in modern portable devices (wearables, smartphones, laptops, tablets). 
They can be used to pick up distinct audio signature's from the environment, such as spoken word, or simply just to measure the level of
ambient noise in an environment.  
  
In SASS, there are two ways in which a microphone can be used to supplement the data already being collected:
analysing speech and ambient noise detection.  
Using a speaker to analyse speech, like Weeks et al. in [2], has shown to be an effective way of detecting SA.
However, this is computationally quite difficult to undertake and isolate the speech for effective analysis. 
This method of SA sensing will be evaluated later in the project, with other features currently taking priority.  
The other way to use the microphone, in this context, is to detect ambient noise. This may prove to be an effective way 
of adding context to the personal data, as ambient noise can be a good indicator of the number of people in the environment, 
as well as whether a particular environment may be especially stressful.

### Accelerometer & Gyroscope - MPU-6050-3 Axis
*Personal/Contextual Data*

### Description
This Inertial Measurement Unit (IMU) is a sensor which is used to detect the movement and orientation of an object. 
IMUs are also ubiquitious in modern portable devices, being used to gather a variety of valuable personal data which can be used 
to provide insights into a users physical activity, orient their screen, or model their movement to aid navigation (with GPS).  
Alike the microphone, the IMU can be used for both Personal and Contextual data with SASS. Although they could both be considered personal, 
the higher-fidelity analysis provides data which is more insightful for the detection of SA. Whereas, the lower-fidelity analysis provides 
more general information regarding the activity of the wearer, which helps add context to their physiological signs.  
The higher-fidelity analysis uses the IMU to detect trembling, or SA specific movements, in the wearer.  
The lower-fidelity analysis uses the IMU to detect general levels of activity, which should indicate how physical exertion the wearer is undergoing. 
Thereby explaining physiological signs which may be heightened as a result of this activity.

## Human Machine Interface
The Human Machine Interface is what allows the user to interact with the system and register when they are feeling SA. 
This is likely to be implemented in a limited form in the lab/bench prototype.

### Push Button

### Microphone

### Gesture/Tap

### Touch Screen/Capacitive Surface

## Communication Modules

### Bluetooth Module - HC-05 Bluetooth RF Transceiver

#### Description
Bluetooth modules are small, low-cost modules which allow for wireless data Radio-Frequency (RF, 2.400 - 2.485 GHz [3]) transfer between devices, 
useful when local storage isn't available or is limited. The modules are also ubiquitious in the modern portable devices previously used, 
making data transfer between devices much simpler, and in the case of wearables allowing for off-board data analysis (on the connected mobile).  
  
The initial module to be used is the HC-05 Bluetooth module. However, Bluetooth Low-Energy (BLE) may be employed later in the project, 
as it reduces the power consumption. This is an important consideration when developing wearable devices as their battery size is restricted.

## Data Storage

### Adafruit MicroSD Card
*This component has been ordered but has not yet arrived*  
  
#### Description
The Adafruit MicroSD Card is a module for additional storage on the Arduino. Additional storage is required as the Arduino has only 32K of 
onboard data storage [4]. As the initial prototype for SASS will be recording data from multiple sensors, it is assumed that additional storage will be required. 
At the point of writing the initial lab prototype has not been assembled yet, and therefore these assumptions have not been validated. 
However, to minimise the chance of complications and delays when developing the prototype it is reasonable to order this inexpensive module.

## References
[1] https://en.wikipedia.org/wiki/Samsung_Galaxy_S5  
[2] Weeks et al. *"The Sound of Fear". Assessing vocal fundamental frequency as a physoplogical indicator
of social anxiety disorder.* [online]. 2012. [Viewed 6 December 2018]. Available from: http://www.sciencedirect.com/science/article/pii/S088761851200093X  
[3] https://en.wikipedia.org/wiki/Bluetooth  
[4] https://www.arduino.cc/en/Tutorial/Memory  
