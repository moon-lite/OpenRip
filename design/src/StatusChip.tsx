export interface StatusChipProps {
  /** True renders the acid-green live state with a pulsing dot; false the dim offline state. */
  online?: boolean;
  /** Short uppercase status text, e.g. "LINKED" or "OFFLINE". */
  label: string;
}

/**
 * Connection status indicator chip with a pulsing dot. Shows device/link
 * state: green pulsing when online, dim grey when offline.
 */
export function StatusChip({ online = false, label }: StatusChipProps) {
  return (
    <span className={online ? "or-chip or-chip--on" : "or-chip"}>
      <span className="or-chip__dot" aria-hidden="true" />
      <span>{label}</span>
    </span>
  );
}
