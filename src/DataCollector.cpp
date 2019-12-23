#include "DataCollector.h"
#include "AQMonitor.h"

DataCollector::DataCollector():
    InfluxDBCollector(&logger,
                      &wifi,
                      &settingsData.influxDB,
                      &settingsData.network) {
}

void DataCollector::collectData(InfluxDBCollector* collector) {
    lastCO2 = co2.getCO2();
    lastTemp = tempSensor.getTemperature() * 10;
    if (lastPushedCO2 < 0) {
        lastPushedCO2 = lastCO2;
        lastPushedTemp = lastTemp;
    }
    collector->append("co2", co2.getCO2());
    collector->append("temperature", tempSensor.getTemperature(), 2);
    collector->append("humidity", tempSensor.getHumidity(), 1);
    collector->append("pressure", tempSensor.getPressure());
}

bool DataCollector::shouldPush() {
    if (lastPushedCO2 < 0) {
        return false;
    }

    if (lastCO2 > 0 && (lastPushedCO2/lastCO2 <= 0.8f || lastPushedCO2/lastCO2 >= 1.25f)) {
        return true;
    }

    if (abs(lastPushedTemp - lastTemp) > 1.0) {
        return true;
    }

    return false;
}

void DataCollector::onPush() {
    lastPushedCO2 = lastCO2;
    lastPushedTemp = lastTemp;
}