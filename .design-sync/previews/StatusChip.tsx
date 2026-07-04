import { StatusChip } from "@openrip/design";

const dark = { background: "var(--or-bg, #0a0b0d)", padding: 24, display: "flex", gap: 12 };

export const Linked = () => (
  <div style={dark}>
    <StatusChip online label="Linked" />
  </div>
);

export const Offline = () => (
  <div style={dark}>
    <StatusChip label="Offline" />
  </div>
);
