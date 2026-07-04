import type { ReactNode } from "react";

export interface BannerProps {
  children: ReactNode;
}

/**
 * Inline warning/notice panel with an accent border on a faint accent wash.
 * For persistent conditions the user should know about (unsupported browser,
 * prototype status) — not for transient toasts.
 */
export function Banner({ children }: BannerProps) {
  return <div className="or-banner">{children}</div>;
}
