// const char* apiKey = "YOUR_API_KEY";  // Replace with your OpenWeatherMap API Key
// const char* zipCode = ZIP_CODE;
// const char* countryCode = COUNTRY_CODE;
// const char* server = "http://api.openweathermap.org/data/2.5/weather?zip=";

// void getData() {
//   if (WiFi.status() == WL_CONNECTED) {
//     HTTPClient http;

//     String url = String(server) + zipCode + "," + countryCode + "&appid=" + apiKey + "&units=metric";  // Metric units

//     http.begin(url);
//     int httpCode = http.GET();

//     if (httpCode > 0) {
//       String payload = http.getString();
//       Serial.println(payload);

//       // Parse JSON
//       StaticJsonDocument<1024> doc;
//       DeserializationError error = deserializeJson(doc, payload);

//       if (error) {
//         Serial.print("deserializeJson() failed: ");
//         Serial.println(error.c_str());
//         return;
//       }

//       float temperature = doc["main"]["temp"];
//       float humidity = doc["main"]["humidity"];

//       Serial.print("Temperature: ");
//       Serial.print(temperature);
//       Serial.println(" °C");

//       Serial.print("Humidity: ");
//       Serial.print(humidity);
//       Serial.println(" %");
//     } else {
//       Serial.print("Error on HTTP request: ");
//       Serial.println(httpCode);
//     }
//     http.end();
//   } else {
//     Serial.println("WiFi not connected");
//   }
// }