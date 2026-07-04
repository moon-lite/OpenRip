import { Gauge } from "@openrip/design";

const dark = { background: "var(--or-bg, #0a0b0d)", padding: 24, display: "grid", placeItems: "center" };

export const Idle = () => (
  <div style={dark}>
    <Gauge value={0} size={220} />
  </div>
);

export const Live = () => (
  <div style={dark}>
    <Gauge value={8450} size={220} />
  </div>
);

export const Redline = () => (
  <div style={dark}>
    <Gauge value={14200} size={220} />
  </div>
);
