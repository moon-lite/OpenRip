import { Button } from "@openrip/design";

const dark = { background: "var(--or-bg, #0a0b0d)", padding: 24, display: "flex", gap: 12 };

export const Primary = () => (
  <div style={dark}>
    <Button variant="primary">Connect</Button>
  </div>
);

export const Ghost = () => (
  <div style={dark}>
    <Button>Export CSV</Button>
    <Button>Sim Launch</Button>
  </div>
);

export const Danger = () => (
  <div style={dark}>
    <Button variant="danger">Clear Log</Button>
  </div>
);

export const Disabled = () => (
  <div style={dark}>
    <Button variant="primary" disabled>Connect</Button>
    <Button disabled>Export CSV</Button>
  </div>
);
