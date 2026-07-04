import { Wordmark } from "@openrip/design";

const dark = { background: "var(--or-bg, #0a0b0d)", padding: 24 };

export const WithSubtitle = () => (
  <div style={dark}>
    <Wordmark subtitle="Launch Telemetry · v0.1" />
  </div>
);

export const Bare = () => (
  <div style={dark}>
    <Wordmark />
  </div>
);
