Final Version:
Über eigenen Wlan Hotspot konfigurieren
 - Authkey ändern
 - Pinkombination ändern (wenn RGB am Lichtstreifen anders angeordnet ist)
DEBUG auskommentieren wenn nicht benötigt
remotecontrol definieren um via http requests variablen zu verändern (zb Farbe, Helligkeit etc.)
RGBSLIDERCODE definieren um eigene RGB Werte am Lichtstreifen auszugeben (Slider müssen in der App extra hinzugefügt werden! Dreifarbige Werte werden am Lichtstreifen sehr weißlich dargestellt)

Um das Wlan zu konfigurieren, 2 mal schnell hintereinander den Reset Knopf drücken.
LED leuchtet jetzt durchgehend.
In der App den Authkey kopieren
mit Wlan "RGB-Controller" verbinden und auf "Im Netzwerk anmelden" klicken.
Jetzt auf "Configure Wifi" klicken,
Wlan Details eingeben
Authkey einfügen
(wenn nötig Pinkombination verändern)
und auf "Save" klicken.

Der RGB Controller ist jetzt fertig eingerichtet.


Virtual Pin Belegung:

V0: 	Menu (1,2,3,4) Farbmodi
V1: 	Delay (1-30) slider
V2: 	manual button (0,1) sliderbutton
V3,4,5: RGB Slider (0-255) 
V7: 	RGB ZEBRA (0-255)
V9: 	brightness slider (1-100)
V10: 	onoff (0,1)

NUR WENN remotecontrol DEFINIERT IST
V30,31,32: 	RGB Remote (0-255)


http://blynk-cloud.com/authkey/update/V(#PIN)?value=(#WERT)
http://blynk-cloud.com/authkey/get/V(#PIN)


Farben stimmen nicht überein? In Web Menü (siehe Wlan konfigurieren) bei "Pin Belegung (optional)" den richtigen Zahlencode für den Lichtstreifen einfügen.

Pin Header  +12V G R B
Colorcode        540      

Pin Header  +12V R G B
Colorcode        450

Pin Header  +12V B G R
Colorcode        054

Pin Header  +12V B R G
Colorcode        045

Pin Header  +12V G B R
Colorcode        504

Pin Header  +12V R B G
Colorcode        405