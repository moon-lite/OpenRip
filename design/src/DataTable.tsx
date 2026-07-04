import type { ReactNode } from "react";

export interface DataTableColumn {
  /** Key into each row object. */
  key: string;
  /** Column header text (rendered uppercase). */
  header: string;
}

export interface DataTableProps {
  columns: DataTableColumn[];
  /** One object per row; values render in the column whose `key` matches. */
  rows: Array<Record<string, ReactNode>>;
  /** Letterspaced empty-state line shown when `rows` is empty. */
  emptyText?: string;
}

/**
 * Mono-numeral data table for logs and history. Uppercase micro-headers,
 * hairline row separators, accent hover wash, and a letterspaced empty
 * state. Horizontally scrolls inside its own border when narrow.
 */
export function DataTable({ columns, rows, emptyText = "NO DATA" }: DataTableProps) {
  return (
    <div className="or-table-wrap">
      <table className="or-table">
        <thead>
          <tr>
            {columns.map((c) => (
              <th key={c.key}>{c.header}</th>
            ))}
          </tr>
        </thead>
        <tbody>
          {rows.map((row, i) => (
            <tr key={i}>
              {columns.map((c) => (
                <td key={c.key}>{row[c.key]}</td>
              ))}
            </tr>
          ))}
        </tbody>
      </table>
      {rows.length === 0 && <div className="or-table__empty">{emptyText}</div>}
    </div>
  );
}
