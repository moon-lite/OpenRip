import type { ReactNode, MouseEventHandler } from "react";

export interface ButtonProps {
  /** Visual weight: `primary` is the filled accent CTA, `ghost` is the quiet bordered default, `danger` warns on hover. */
  variant?: "primary" | "ghost" | "danger";
  disabled?: boolean;
  onClick?: MouseEventHandler<HTMLButtonElement>;
  type?: "button" | "submit" | "reset";
  children: ReactNode;
}

/**
 * Pill action button. `primary` is the solid ink pill (teal on hover) — one
 * per screen (e.g. Connect); `ghost` is a quiet hairline pill for secondary
 * actions (Export CSV); `danger` warns coral on hover for destructive ones
 * (Clear).
 */
export function Button({ variant = "ghost", disabled, onClick, type = "button", children }: ButtonProps) {
  const cls = variant === "ghost" ? "or-btn" : `or-btn or-btn--${variant}`;
  return (
    <button className={cls} disabled={disabled} onClick={onClick} type={type}>
      {children}
    </button>
  );
}
