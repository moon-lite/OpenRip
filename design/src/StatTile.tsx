import type { ReactNode } from "react";

export interface StatTileProps {
  /** Small uppercase caption, e.g. "SESSION BEST". */
  label: string;
  /** The stat itself — a number, "—" for empty, or any short node. */
  value: ReactNode;
}

/**
 * Boxed metric tile: tiny uppercase label over a large tabular-numeral
 * value. Compose several in a grid for session stats; use "—" as the value
 * for not-yet-available data.
 */
export function StatTile({ label, value }: StatTileProps) {
  return (
    <div className="or-stat">
      <div className="or-stat__label">{label}</div>
      <div className="or-stat__value">{value}</div>
    </div>
  );
}
