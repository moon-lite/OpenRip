export interface WordmarkProps {
  /** Optional letterspaced subtitle line, e.g. "LAUNCH TELEMETRY · v0.1". */
  subtitle?: string;
}

/**
 * The OPENRIP logotype: "OPEN" in ink, "RIP" in glowing accent orange, with
 * an optional letterspaced subtitle. Belongs at the top-left of a screen.
 */
export function Wordmark({ subtitle }: WordmarkProps) {
  return (
    <div className="or-wordmark">
      <span>OPEN</span>
      <span className="or-wordmark__rip">RIP</span>
      {subtitle && <span className="or-wordmark__sub">{subtitle}</span>}
    </div>
  );
}
