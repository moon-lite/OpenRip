#pragma once

#include <stdint.h>
#include "launch_record.h"

// BLE GATT service (SPEC.md §4) — M1 feature, stubbed for M0.
//
// Build with -DOPENRIP_ENABLE_BLE (the `ble-stub` PlatformIO env) to compile
// the GATT server; the default M0 build compiles these as no-ops so the
// firmware stays serial-only.

// Initialise the GATT server and start advertising.
void bleInit();

// Push a completed launch record over the notify characteristic.
void bleNotifyLaunch(const launch_record_t &rec);

// Update the live RPM stream characteristic (training mode).
void bleUpdateLiveRpm(uint16_t rpm);
