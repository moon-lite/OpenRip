import { StatTile } from "@openrip/design";

const dark = { background: "var(--or-bg, #0a0b0d)", padding: 24 };

export const Single = () => (
  <div style={dark}>
    <StatTile label="Session Best" value="11,240" />
  </div>
);

export const StatsGrid = () => (
  <div style={{ ...dark, display: "grid", gridTemplateColumns: "1fr 1fr", gap: 20 }}>
    <StatTile label="Session Best" value="11,240" />
    <StatTile label="Session Avg" value="9,872" />
    <StatTile label="Launches" value="14" />
    <StatTile label="Battery" value="87%" />
  </div>
);

export const Empty = () => (
  <div style={dark}>
    <StatTile label="Session Best" value="—" />
  </div>
);
