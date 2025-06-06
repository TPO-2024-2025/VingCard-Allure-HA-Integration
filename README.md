# VingCard Allure Home Assitant Integration

VingCard Allure integracija omogoča vpogled v stanja vseh hotelskih sob preko Home Assistant aplikacije. Hotelsko osebje lahko na ta način hitro in učinkovito spremlja spremembe stanja sob.

[YouTube povezava](https://youtu.be/ma-Vmfjnda0)<br>
[Prezentacija](https://docs.google.com/presentation/d/1qRhhchc4pXnYk-6he5H4j2wIlV2fJX2RX7SH2XOMz2w/edit?usp=sharing)

## Zahteve

- Vingcard Allure plošča (1x na sobo)
- ESP8266 mikrokontroler (1x na sobo)
- [Mosquitto](https://mosquitto.org/)
- [Arduino IDE](https://www.arduino.cc/)
- [Home Assistant](https://www.home-assistant.io/)
- [Docker](https://www.docker.com/)

## Namestitev

Izvorno kodo najprej prenesemo:
```bash
git clone https://github.com/TPO-2024-2025/VingCard-Allure-HA-Integration.git
```

## Uporaba

### Vzpostative Home Assistant-a

Home Assistant vzpostavimo s pomočjo docker-ja

V direktoriju [home assistanta](./ha-vingcard-dev/) zaženemo ukaza
```bash
docker-compose build --no-cache
docker-compose up
```

### Konfiguriranje MQTT broker-ja

V direktoriju, kjer je nameščen Mosquitto, dodamo naslednje vrstice na dno `mosquitto.conf`:
```
allow_anonymous false
listener 1883
password_file absolute_path_to_passwd.txt
```

V istem direktoriju prav tako ustvarimo nov file `passwd.txt`, v katerega bomo zapisali vsa uporabniška imena in gesla pošiljateljev in odjemalca.<br>
Z naslednjim ukazom dodamo uporabniško ime in geslo za vse sobe in odjemalca
```bash
mosquitto_passwd -b path_to_passwd.txt username password
```

Za zagon brokerja poženemo ukaz
```bash
mosquitto -c path_to_moquitto.conf
```

### Vzpostavitev MQTT odjemalca

Z najljubšim urejevalnikom besedila odpremo [izvorno kodo MQTT broker-ja](./src/backend/subscriber.py).<br>
Pomembna je nastavitev IP naslova naprave, na katerem poganjamo MQTT broker
```python
BROKER_IP = "192.168.43.212"
```

Po tem, odjemalca poženemo z ukazom
```bash
python subscriber.py
```

### Programiranje mikrokontrolerja

V Arduino IDE-ju odpremo [kodo za mikrokontroler](./src/sensor/esp8266MOD/esp8266MOD.ino).<br>


#### Knižnjice

V urejevalcu knižnjic Arduino IDE-ja namestimo knižnjice za MQTT in WiFi:
- PubSubClient
- ESP8266WiFi

<br>

Na vrhu programa se nahajajo pomembne nastavitve, ki jih je potrebno nastaviti preden mikrokontroler programiramo.

#### WiFi SSID
SSID WiFi omrežja na katerega se bo mikrokontroler povezal. Povezava je potrebna za pošiljanje MQTT sporočil.
```cpp
const char* wifiSsid = "IHGnetwork";
```

#### WiFi geslo
Če je omrežje zaščiteno z geslom (priporočeno), ga zapišemo tukaj
```cpp
const char* wifiPassword = "password123";
```

#### IP naslov MQTT broker-ja
IP naslov naprave, na kateri je pognan [MQTT broker](./src/backend/subscriber.py)
```cpp
const char* ipAddress = "192.168.43.212";
```

#### ID mikrokontrolerja
Identita, ki mora bit unikaten za vsako napravo. Predlagan format je soba{številka sobe}
Primer: soba102
```cpp
const char* publisherId = "soba102";
```

#### Ime teme
Za ime teme, kamor mikrokontroler pošilja podatke, uporabimo le številko sobe
Primer: 102
```cpp
const char* publisherTopic = "101";
```

#### Ime mikrokontrolerja
Ime, ki je potrebno za avtentikacijo. Priporočeno je uporabiti isto ime, kot za ID
Primer: soba102
```cpp
const char* publisherUsername = "soba102";
```

#### Geslo mikrokontrolerja
Geslo, ki je potrebno za avtentikacijo.
Primer: 201abos
```cpp
const char* publisherPassword = "201abos";
```

Naprava je sedaj pripravljena za programiranje. IDE naredi to zelo preprosto, pritisnemo desno usmerjeno puščico za nalaganje (Upload) in počakamo da se koda namesti. V serial konzoli lahko spremljamo stanje naprave.