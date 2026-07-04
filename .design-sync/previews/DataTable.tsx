import { DataTable } from "@openrip/design";

const dark = { background: "var(--or-bg, #0a0b0d)", padding: 24 };

const columns = [
  { key: "n", header: "#" },
  { key: "peak", header: "Peak RPM" },
  { key: "rip", header: "Rip" },
  { key: "batt", header: "Batt" },
  { key: "logged", header: "Logged" },
];

export const LaunchLog = () => (
  <div style={dark}>
    <DataTable
      columns={columns}
      rows={[
        { n: "4", peak: "11,240", rip: "512 ms", batt: "87%", logged: "6:42:19 PM" },
        { n: "3", peak: "10,861", rip: "534 ms", batt: "88%", logged: "6:41:02 PM" },
        { n: "2", peak: "9,406", rip: "488 ms", batt: "88%", logged: "6:39:47 PM" },
        { n: "1", peak: "10,118", rip: "561 ms", batt: "89%", logged: "6:38:15 PM" },
      ]}
    />
  </div>
);

export const EmptyState = () => (
  <div style={dark}>
    <DataTable columns={columns} rows={[]} emptyText="No launches logged — let it rip" />
  </div>
);
