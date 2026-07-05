export interface WordmarkProps {
  /** Optional italic subtitle line, e.g. "launch telemetry · vol 0.1". */
  subtitle?: string;
}

/**
 * The open=rip logotype: lowercase ink wordmark with a teal equals sign —
 * zine-style. Optional italic subtitle underneath. Belongs at the top-left
 * of a screen.
 */
export function Wordmark({ subtitle }: WordmarkProps) {
  return (
    <div className="or-wordmark">
      <span>open</span>
      <span className="or-wordmark__eq">=</span>
      <span className="or-wordmark__rip">rip</span>
      {subtitle && <span className="or-wordmark__sub">{subtitle}</span>}
    </div>
  );
}
