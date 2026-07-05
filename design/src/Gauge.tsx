import type { CSSProperties } from "react";

export interface GaugeProps {
  /** Current value shown in the center and swept as the arc. */
  value: number;
  /** Full-scale value for a complete 360° sweep. Default 15000. */
  max?: number;
  /** Caption under the number, e.g. "RPM · LIVE". */
  unit?: string;
  /** Diameter in px. Default 280. */
  size?: number;
}

/**
 * Circular gauge — the hero readout. A teal conic arc sweeps a light track
 * as `value` approaches `max`; the numeral turns teal whenever the value is
 * non-zero (data is live). Use for the primary live metric on a screen;
 * pair with StatTile for secondary numbers.
 */
export function Gauge({ value, max = 15000, unit = "RPM · LIVE", size = 280 }: GaugeProps) {
  const deg = Math.min(Math.max(value, 0) / max, 1) * 360;
  const style = { "--or-gauge-fill": `${deg}deg`, width: size } as CSSProperties;
  return (
    <div className={value > 0 ? "or-gauge or-gauge--live" : "or-gauge"} style={style}>
      <div className="or-gauge__inner">
        <div className="or-gauge__value">{new Intl.NumberFormat("en-US").format(Math.round(value))}</div>
        <div className="or-gauge__unit">{unit}</div>
      </div>
    </div>
  );
}
