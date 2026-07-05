import type { ReactNode } from "react";

export interface StatTileProps {
  /** Small uppercase caption, e.g. "SESSION BEST". */
  label: string;
  /** The stat itself — a number, "—" for empty, or any short node. */
  value: ReactNode;
}

/**
 * Editorial stat block: a bold top rule, small lowercase label, and a large
 * tabular numeral — like a magazine index entry. Compose several in a row
 * or grid for session stats; use "—" as the value for not-yet-available data.
 */
export function StatTile({ label, value }: StatTileProps) {
  return (
    <div className="or-stat">
      <div className="or-stat__label">{label}</div>
      <div className="or-stat__value">{value}</div>
    </div>
  );
}
