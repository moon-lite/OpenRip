import type { ReactNode } from "react";

export interface BannerProps {
  children: ReactNode;
}

/**
 * Margin-note panel: a teal left rule on a faint teal wash, italic text —
 * like an editor's note. For persistent conditions the user should know
 * about (unsupported browser, prototype status) — not for transient toasts.
 */
export function Banner({ children }: BannerProps) {
  return <div className="or-banner">{children}</div>;
}
