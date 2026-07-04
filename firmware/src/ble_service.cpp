#include "ble_service.h"

#ifdef OPENRIP_ENABLE_BLE

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Draft UUIDs — placeholders until the protocol is frozen at M1 (SPEC §9 Q3
// leans toward our own documented schema rather than mimicking the official
// BattlePass protocol).
static const char *SERVICE_UUID          = "6f7a0001-52c1-4a8e-b8a5-0e5f1c9d2b00";
static const char *CHAR_LAUNCH_UUID      = "6f7a0002-52c1-4a8e-b8a5-0e5f1c9d2b00"; // notify: launch_record_t
static const char *CHAR_LIVE_RPM_UUID    = "6f7a0003-52c1-4a8e-b8a5-0e5f1c9d2b00"; // notify: uint16 live RPM
static const char *DEVICE_NAME           = "OpenRip";

static BLECharacteristic *g_launchChar  = nullptr;
static BLECharacteristic *g_liveRpmChar = nullptr;
static volatile bool g_clientConnected  = false;

class ServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer *) override { g_clientConnected = true; }
    void onDisconnect(BLEServer *server) override {
        g_clientConnected = false;
        server->getAdvertising()->start();
    }
};

void bleInit() {
    BLEDevice::init(DEVICE_NAME);
    BLEServer *server = BLEDevice::createServer();
    server->setCallbacks(new ServerCallbacks());

    BLEService *service = server->createService(SERVICE_UUID);

    g_launchChar = service->createCharacteristic(
        CHAR_LAUNCH_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    g_launchChar->addDescriptor(new BLE2902());

    g_liveRpmChar = service->createCharacteristic(
        CHAR_LIVE_RPM_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    g_liveRpmChar->addDescriptor(new BLE2902());

    service->start();

    BLEAdvertising *adv = server->getAdvertising();
    adv->addServiceUUID(SERVICE_UUID);
    adv->start();
}

void bleNotifyLaunch(const launch_record_t &rec) {
    if (!g_launchChar || !g_clientConnected) return;
    g_launchChar->setValue((uint8_t *)&rec, sizeof(rec));
    g_launchChar->notify();
}

void bleUpdateLiveRpm(uint16_t rpm) {
    if (!g_liveRpmChar || !g_clientConnected) return;
    g_liveRpmChar->setValue((uint8_t *)&rpm, sizeof(rpm));
    g_liveRpmChar->notify();
}

bool bleIsConnected() { return g_clientConnected; }

#else // serial-only M0 build

void bleInit() {}
void bleNotifyLaunch(const launch_record_t &) {}
void bleUpdateLiveRpm(uint16_t) {}
bool bleIsConnected() { return false; }

#endif // OPENRIP_ENABLE_BLE
