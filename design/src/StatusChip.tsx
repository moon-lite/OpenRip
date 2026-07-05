export interface StatusChipProps {
  /** True renders the teal live state with a pulsing dot; false the dim offline state. */
  online?: boolean;
  /** Short status text, e.g. "Linked" or "Offline". */
  label: string;
}

/**
 * Connection status pill with a pulsing dot. Shows device/link state:
 * teal pulsing when online, dim grey when offline.
 */
export function StatusChip({ online = false, label }: StatusChipProps) {
  return (
    <span className={online ? "or-chip or-chip--on" : "or-chip"}>
      <span className="or-chip__dot" aria-hidden="true" />
      <span>{label}</span>
    </span>
  );
}
