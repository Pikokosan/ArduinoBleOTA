#pragma once
#include "BleOtaSecurity.h"
#ifdef USE_NIM_BLE_ARDUINO_LIB
#include <NimBLEDevice.h>
#include <Preferences.h>

#define BLE_OTA_SECURITY_DICT "ota_security"
#define BLE_OTA_PIN_KEY "ota_pin"

class BleOtaSecurityOnConnect : public BleOtaSecurity,
                                public BLEServerCallbacks
{
public:
    void begin()
    {
        if (not prefs.begin(BLE_OTA_SECURITY_DICT))
            return;

        if (not prefs.isKey(BLE_OTA_PIN_KEY))
        {
            prefs.end();
            return;
        }

        BLEDevice::setSecurityPasskey(prefs.getUInt(BLE_OTA_PIN_KEY));
        BLEDevice::setSecurityAuth(true, true, true);
        BLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);
        auto* server = BLEDevice::createServer();
        server->setCallbacks(this);
        prefs.end();
    }

    bool setPin(uint32_t pin) override
    {
        if (not prefs.begin(BLE_OTA_SECURITY_DICT))
            return false;

        const bool result = prefs.putUInt(BLE_OTA_PIN_KEY, pin);

        prefs.end();
        return result;
    }

    bool removePin() override
    {
        if (not prefs.begin(BLE_OTA_SECURITY_DICT))
            return false;

        const bool result = not prefs.isKey(BLE_OTA_PIN_KEY) or
                            prefs.remove(BLE_OTA_PIN_KEY);

        prefs.end();
        return result;
    }

    void onConnect(BLEServer* srv, ble_gap_conn_desc* desc) override
    {
        BLEDevice::startSecurity(desc->conn_handle);
    }

private:
    Preferences prefs;
};

#endif