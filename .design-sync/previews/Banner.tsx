import { Banner } from "@openrip/design";

const dark = { background: "var(--or-bg, #0a0b0d)", padding: 24 };

export const BrowserWarning = () => (
  <div style={dark}>
    <Banner>
      This browser has no Web Bluetooth — use Chrome or Edge on desktop.
      Sim mode still works below.
    </Banner>
  </div>
);

export const PrototypeNotice = () => (
  <div style={dark}>
    <Banner>
      <strong>Early prototype.</strong> Sensor placement hasn&apos;t been
      validated on a real launcher yet.
    </Banner>
  </div>
);
