// #include "dht11_helper.h"
//
//     PietteTech_DHT *_sensor;
// void _dhtWrapper();
// bool _started = false;
//
// // DhtHelper::DhtHelper(bool enabled, uint8_t pin)
// DhtHelper::DhtHelper(bool enabled, uint8_t pin, uint8_t type)
// {
//   _enabled = enabled;
//   _lastHumidity = 0;
//   _lastTemp = 0;
//   if (enabled) {
//     PietteTech_DHT sensor(pin, type, _dhtWrapper);
//     // Dht11 sensor(pin);
//     _sensor = &sensor;
//     _lastCheck = millis();
//   }
// }
//
// void _dhtWrapper() {
//   _sensor->isrCallback();
// }
//
// void DhtHelper::onHumidityChange(changeCallback fn) {
//   _humidityCallback = fn;
// }
//
// void DhtHelper::onTempChange(changeCallback fn) {
//   _tempCallback = fn;
// }
//
// void DhtHelper::begin()
// {
//   // _sensor->begin();
// }
//
// void DhtHelper::loop()
// {
//   unsigned long now = millis();
//   unsigned long timeSinceLastCheck = now - _lastCheck;
//   // If we haven't hit the poll timeout don't do anything.
//   if (timeSinceLastCheck < pollRate) {
//     return;
//   }
//   Serial.println("Starting read");
//   _lastCheck = now;
//
//   // switch (_sensor->read()) {
//   //  case Dht11::OK:
//   //  {
//   //      Serial.print("Humidity (%): ");
//   //      int h = _sensor->getHumidity();
//   //
//   //      Serial.print("Temperature (C): ");
//   //      float t = _sensor->getTemperature();
//   //     if (h != _lastHumidity) {
//   //       if (_humidityCallback != nullptr) {
//   //         _humidityCallback(h);
//   //       }
//   //       _lastHumidity = h;
//   //     }
//   //     if (t != _lastTemp) {
//   //       if (_tempCallback != nullptr) {
//   //         _tempCallback(t);
//   //       }
//   //       _lastTemp = t;
//   //     }
//   //      break;
//   //    }
//   //
//   //  case Dht11::ERROR_CHECKSUM:
//   //      Serial.println("Checksum error");
//   //     break;
//   //
//   //  case Dht11::ERROR_TIMEOUT:
//   //      Serial.println("Timeout error");
//   //     break;
//   //
//   //  default:
//   //      Serial.println("Unknown error");
//   //     break;
//   //  }
//
//   // blocking method
//    int acquirestatus = 0;
//    int acquireresult = _sensor->acquireAndWait(1000);
//    if ( acquireresult == 0 ) {
//      float t = _sensor->getCelsius();
//       Serial.println("read temp");
//      float h = _sensor->getHumidity();
//       Serial.println("read humidity");
//     //  d = DHT.getDewPoint();
//       if (isnan(h) || isnan(t)) {
//        Serial.println("Failed to read from DHT sensor!");
//        return;
//      }
//
//     if (h != _lastHumidity) {
//       if (_humidityCallback != nullptr) {
//         _humidityCallback(h);
//       }
//       _lastHumidity = h;
//     }
//     if (t != _lastTemp) {
//       if (_tempCallback != nullptr) {
//         _tempCallback(t);
//       }
//       _lastTemp = t;
//     }
//   } else {
//     Serial.println("invalid acquire result");
//     Serial.println(acquireresult == DHTLIB_ERROR_NOTSTARTED);
//     Serial.println(acquireresult - 5);
//   }
// }
