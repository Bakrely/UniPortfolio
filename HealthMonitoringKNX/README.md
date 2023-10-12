‘Development of a low cost Health Monitoring System for KNX and integration in a Smart Home Platform’ 
Sensors: 
• Room parameters (continuous measurement) o Humidity and temperature 
 
o C02 
https://learn.adafruit.com/dht 
https://www.tinytronics.nl/shop/en/sensors/air/gas/winsen-mh-z19b-co2- sensor-with-cable https://www.antratek.com/co2-and-rh-t-sensor-scd30 
    
• Patient parameters (on demand measurement, valid for limited time) o Heart Rate / Pulse Oximeter Sensor / Temperature 
https://www.tinytronics.nl/shop/nl/sensoren/lichaam/hartslagsensor-en- pulsoxymeter-max30102-i2c https://how2electronics.com/iot-based-patient-health-monitoring-esp32- web-server/ 
o Temperature https://www.adafruit.com/product/381 
Integration in the KNX system: 
• http://www.opternus.com/en/siemens/development-tools/tp-uart2-board-btm2-pcb.html • http://liwan.fr/KnxWithArduino/ • https://github.com/knxd/knxd/wiki 
More info: 
• https://www.bauwas.eu/?p=928 • https://www.auto.tuwien.ac.at/~mkoegler/index.php/bcusdk • http://busware.de/tiki-index.php?page=TUL • https://groups.google.com/g/knxd • https://schema.knx.org/2020/api/v1 
Smart Home Platforms: 
	•	Gira Homeserver  
	•	Gira X1  
	•	Domovea  
	•	Zennio  
	•	Raspberry Pi based  
o Home Assistant 
o Homebridge o OpenHAB 
Assignment: 
• Each team selects at least 1 room and 1 patient sensor, interfaces to KNX (together with other KNX components) and integrates in a Smart Home Platform of choice 