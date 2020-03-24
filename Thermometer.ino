double readTemp() {
  return dht.readTemperature();
}

double readHum() {
  return dht.readHumidity();
}
