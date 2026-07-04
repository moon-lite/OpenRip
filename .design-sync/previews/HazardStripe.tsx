import { HazardStripe, Wordmark } from "@openrip/design";

export const HeaderDivider = () => (
  <div style={{ background: "var(--or-bg, #0a0b0d)", padding: 24 }}>
    <div style={{ paddingBottom: 16 }}>
      <Wordmark subtitle="Launch Telemetry · v0.1" />
    </div>
    <HazardStripe />
  </div>
);
