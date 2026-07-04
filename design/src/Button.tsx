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
 * Industrial clipped-corner action button. Uppercase display type, angular
 * clip-path corners. Use `primary` for the single main action on a screen
 * (e.g. CONNECT), `ghost` for secondary actions (EXPORT CSV), `danger` for
 * destructive ones (CLEAR).
 */
export function Button({ variant = "ghost", disabled, onClick, type = "button", children }: ButtonProps) {
  const cls = variant === "ghost" ? "or-btn" : `or-btn or-btn--${variant}`;
  return (
    <button className={cls} disabled={disabled} onClick={onClick} type={type}>
      {children}
    </button>
  );
}
