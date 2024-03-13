/*
 * Projet : Connexion d'un ESP32 ou ESP8266 à InfluxDB
 * Description : Ce code permet à un appareil ESP (ESP32 ou ESP8266) d'établir une connexion à la base de données InfluxDB.
 * 
 * Auteur : Richedor LALEYE (Adecola)
 * Date : 7 mars 2024
 * Version : 1
 * Licence : MIT
 */

#if defined(ESP32)
  // Inclusion de la bibliothèque WiFiMulti pour la gestion de plusieurs points d'accès Wi-Fi (pour ESP32)
  #include <WiFiMulti.h>
  WiFiMulti wifiMulti;
  #define DEVICE "ESP32"  // Définition du nom de l'appareil (ESP32)
#elif defined(ESP8266)
  // Inclusion de la bibliothèque ESP8266WiFiMulti pour la gestion de plusieurs points d'accès Wi-Fi (pour ESP8266)
  #include <ESP8266WiFiMulti.h>
  ESP8266WiFiMulti wifiMulti;
  #define DEVICE "ESP8266"  // Définition du nom de l'appareil (ESP8266)
#endif

// Inclusion des bibliothèques nécessaires pour la communication avec InfluxDB
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

// Informations de connexion Wi-Fi
#define WIFI_SSID "GKA_WIFI_1"   // Nom du réseau Wi-Fi
#define WIFI_PASSWORD "GreenKA2020"  // Mot de passe du réseau Wi-Fi

// Informations de connexion à InfluxDB
#define INFLUXDB_URL "https://us-east-1-1.aws.cloud2.influxdata.com" // URL de la base de données
#define INFLUXDB_TOKEN "PUCbT1zgHeE5HjUs7vFs2bCMo9KduUl26emitlEL7ZOQbDjxOiJNykoZwIAL-JAZaeEuMUMs-37MvLfpLC9Byw=="  // Jeton d'accès
#define INFLUXDB_ORG "24355fe20890a9f8"   // Organisation InfluxDB
#define INFLUXDB_BUCKET "ESP32"       // Bucket InfluxDB

// Informations de fuseau horaire
#define TZ_INFO "UTC1"  // Fuseau horaire (UTC+1)

// Création d'une instance du client InfluxDB avec un certificat préconfiguré pour InfluxCloud
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Déclaration d'un point de données nommé "wifi_status" pour stocker l'état de la connexion Wi-Fi
Point sensor("wifi_status");

void setup() {
  Serial.begin(115200);  // Initialisation de la communication série à 115200 bauds

  // Configuration Wi-Fi
  WiFi.mode(WIFI_STA);  // Réglage du mode Wi-Fi en mode station (client)
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);  // Ajout du réseau Wi-Fi aux points d'accès gérés

  Serial.print("Connexion au Wi-Fi...");
  while (wifiMulti.run() != WL_CONNECTED) {  // Boucle d'attente de la connexion Wi-Fi
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  // Synchronisation de l'heure pour une validation de certificat et une écriture en lots correctes
  // Utilise les serveurs NTP de votre région comme fournis par https://www.pool.ntp.org/zone/
  // L'état de la synchronisation et l'heure seront imprimés sur la console série.
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  // Vérification de la connexion au serveur InfluxDB
  if (client.validateConnection()) {
    Serial.print("Connecté à InfluxDB : ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("Échec de connexion à InfluxDB : ");
    Serial.println(client.
