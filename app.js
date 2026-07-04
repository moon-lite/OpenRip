// OpenRip Web Bluetooth app — v0.1 (SPEC.md §5)
// Connect, live RPM, launch history in localStorage, CSV export.
// UUIDs and the 10-byte packed launch record must match
// firmware/include/{ble_service.h-adjacent constants, launch_record.h}.

"use strict";

// --- Protocol (keep in sync with firmware/src/ble_service.cpp) ---------------

const SERVICE_UUID   = "6f7a0001-52c1-4a8e-b8a5-0e5f1c9d2b00";
const CHAR_LAUNCH    = "6f7a0002-52c1-4a8e-b8a5-0e5f1c9d2b00";
const CHAR_LIVE_RPM  = "6f7a0003-52c1-4a8e-b8a5-0e5f1c9d2b00";

const GAUGE_MAX_RPM  = 15000;   // full-scale on the dial; matches --gauge-max
const LIVE_DECAY_MS  = 600;     // zero the live readout after this long with no data
const HISTORY_KEY    = "openrip.history.v1";
const HISTORY_CAP    = 500;

// launch_record_t, little-endian, 10 bytes (firmware/include/launch_record.h)
function parseLaunchRecord(dataView) {
  if (dataView.byteLength < 10) return null;
  return {
    seq:               dataView.getUint16(0, true),
    peak_rpm:          dataView.getUint16(2, true),
    release_pitch_deg: dataView.getInt8(4),
    release_roll_deg:  dataView.getInt8(5),
    rip_duration_ms:   dataView.getUint16(6, true),
    smoothness:        dataView.getUint8(8),
    battery_pct:       dataView.getUint8(9),
  };
}

// --- DOM ----------------------------------------------------------------------

const $ = (id) => document.getElementById(id);
const el = {
  chip: $("status-chip"), status: $("status-text"), connect: $("btn-connect"),
  gauge: $("gauge"), rpm: $("rpm-live"),
  best: $("stat-best"), avg: $("stat-avg"), count: $("stat-count"), batt: $("stat-batt"),
  body: $("history-body"), empty: $("history-empty"),
  sim: $("btn-sim"), csv: $("btn-csv"), clear: $("btn-clear"),
  unsupported: $("banner-unsupported"),
};

// --- State ----------------------------------------------------------------------

let device = null;
let history = loadHistory();
let liveDecayTimer = null;

function loadHistory() {
  try {
    const raw = localStorage.getItem(HISTORY_KEY);
    const arr = raw ? JSON.parse(raw) : [];
    return Array.isArray(arr) ? arr : [];
  } catch {
    return [];
  }
}

function saveHistory() {
  if (history.length > HISTORY_CAP) history = history.slice(-HISTORY_CAP);
  try {
    localStorage.setItem(HISTORY_KEY, JSON.stringify(history));
  } catch { /* storage full/blocked — keep going in-memory */ }
}

// --- Rendering --------------------------------------------------------------------

const fmt = new Intl.NumberFormat("en-US");

function setLiveRpm(rpm) {
  el.rpm.textContent = fmt.format(rpm);
  el.rpm.classList.toggle("hot", rpm > 0);
  const deg = Math.min(rpm / GAUGE_MAX_RPM, 1) * 360;
  el.gauge.style.setProperty("--fill", `${deg}deg`);

  clearTimeout(liveDecayTimer);
  if (rpm > 0) liveDecayTimer = setTimeout(() => setLiveRpm(0), LIVE_DECAY_MS);
}

function setStatus(connected, label) {
  el.chip.classList.toggle("chip--on", connected);
  el.chip.classList.toggle("chip--off", !connected);
  el.status.textContent = label;
  el.connect.textContent = connected ? "DISCONNECT" : "CONNECT";
}

function renderStats() {
  const peaks = history.map((r) => r.peak_rpm);
  el.count.textContent = String(history.length);
  el.best.textContent = peaks.length ? fmt.format(Math.max(...peaks)) : "—";
  el.avg.textContent = peaks.length
    ? fmt.format(Math.round(peaks.reduce((a, b) => a + b, 0) / peaks.length))
    : "—";
  const last = history[history.length - 1];
  el.batt.textContent = last ? `${last.battery_pct}%` : "—";
}

function renderHistory() {
  el.body.textContent = "";
  // newest first
  for (let i = history.length - 1; i >= 0; i--) {
    const r = history[i];
    const tr = document.createElement("tr");
    const t = new Date(r.logged_at);
    const cells = [
      ["", String(i + 1)],
      ["peak", fmt.format(r.peak_rpm)],
      ["dim", `${r.rip_duration_ms} ms`],
      ["dim", `${r.battery_pct}%`],
      ["dim", isNaN(t) ? "—" : t.toLocaleTimeString()],
      ["dim", r.source === "sim" ? "SIM" : "BLE"],
    ];
    for (const [cls, text] of cells) {
      const td = document.createElement("td");
      if (cls) td.className = cls;
      td.textContent = text;
      tr.appendChild(td);
    }
    el.body.appendChild(tr);
  }
  el.empty.hidden = history.length > 0;
  const first = el.body.firstElementChild;
  if (first) first.classList.add("fresh");
}

function logLaunch(record, source) {
  history.push({ ...record, logged_at: new Date().toISOString(), source });
  saveHistory();
  renderStats();
  renderHistory();
}

// --- BLE ------------------------------------------------------------------------------

async function connect() {
  device = await navigator.bluetooth.requestDevice({
    filters: [{ services: [SERVICE_UUID] }, { namePrefix: "OpenRip" }],
    optionalServices: [SERVICE_UUID],
  });
  device.addEventListener("gattserverdisconnected", () => {
    setStatus(false, "OFFLINE");
    setLiveRpm(0);
  });

  setStatus(false, "LINKING…");
  const server = await device.gatt.connect();
  const service = await server.getPrimaryService(SERVICE_UUID);

  const launchChar = await service.getCharacteristic(CHAR_LAUNCH);
  launchChar.addEventListener("characteristicvaluechanged", (e) => {
    const rec = parseLaunchRecord(e.target.value);
    if (rec) logLaunch(rec, "ble");
  });
  await launchChar.startNotifications();

  const liveChar = await service.getCharacteristic(CHAR_LIVE_RPM);
  liveChar.addEventListener("characteristicvaluechanged", (e) => {
    if (e.target.value.byteLength >= 2) setLiveRpm(e.target.value.getUint16(0, true));
  });
  await liveChar.startNotifications();

  setStatus(true, "LINKED");
}

async function onConnectClick() {
  if (device?.gatt?.connected) {
    device.gatt.disconnect();
    return;
  }
  el.connect.disabled = true;
  try {
    await connect();
  } catch (err) {
    setStatus(false, "OFFLINE");
    if (err?.name !== "NotFoundError") { // NotFoundError = user closed the picker
      console.error("[openrip]", err);
      alert(`Connection failed: ${err.message ?? err}`);
    }
  } finally {
    el.connect.disabled = false;
  }
}

// --- Sim mode (UI testing without hardware) -----------------------------------------------

let simSeq = 0;
let simRunning = false;

function simLaunch() {
  if (simRunning) return;
  simRunning = true;
  const peak = 5000 + Math.round(Math.random() * 6000);
  const durationMs = 400 + Math.round(Math.random() * 400);
  const t0 = performance.now();

  (function tick() {
    const t = (performance.now() - t0) / durationMs;
    if (t >= 1) {
      setLiveRpm(peak);
      logLaunch({
        seq: simSeq++,
        peak_rpm: peak,
        release_pitch_deg: 0,
        release_roll_deg: 0,
        rip_duration_ms: durationMs,
        smoothness: 0,
        battery_pct: 100,
      }, "sim");
      simRunning = false;
      return;
    }
    // ease-out ramp, a little jitter like a real rip
    const rpm = Math.round(peak * (1 - Math.pow(1 - t, 3)) * (0.96 + Math.random() * 0.04));
    setLiveRpm(rpm);
    requestAnimationFrame(tick);
  })();
}

// --- CSV export ------------------------------------------------------------------------------

function exportCsv() {
  const cols = ["seq", "peak_rpm", "rip_duration_ms", "release_pitch_deg",
                "release_roll_deg", "smoothness", "battery_pct", "logged_at", "source"];
  const lines = [cols.join(",")];
  for (const r of history) lines.push(cols.map((c) => r[c] ?? "").join(","));
  const blob = new Blob([lines.join("\n") + "\n"], { type: "text/csv" });
  const a = document.createElement("a");
  a.href = URL.createObjectURL(blob);
  a.download = `openrip-launches-${new Date().toISOString().slice(0, 10)}.csv`;
  a.click();
  URL.revokeObjectURL(a.href);
}

// --- Wire up ------------------------------------------------------------------------------------

el.connect.addEventListener("click", onConnectClick);
el.sim.addEventListener("click", simLaunch);
el.csv.addEventListener("click", exportCsv);
el.clear.addEventListener("click", () => {
  if (!history.length || confirm(`Delete all ${history.length} logged launches?`)) {
    history = [];
    saveHistory();
    renderStats();
    renderHistory();
  }
});

if (!navigator.bluetooth) {
  el.unsupported.hidden = false;
  el.connect.disabled = true;
}

renderStats();
renderHistory();
setLiveRpm(0);
