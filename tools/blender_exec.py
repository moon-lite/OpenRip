#!/usr/bin/env python3
"""Execute Python code inside Blender via the official Blender Lab MCP addon
(null-byte-delimited JSON over TCP, default port 9876).

Usage:
  python3 bx.py <file.py>     # run code from a file
  python3 bx.py -             # run code from stdin
Protocol: {"type": "execute", "code": ..., "strict_json": false} + "\0"
The in-Blender namespace convention: set `result = {...}` for structured
output; stdout/stderr are captured and returned too.
"""
import json
import socket
import sys
import time

HOST, PORT = "127.0.0.1", 9876
TIMEOUT = float(sys.argv[2]) if len(sys.argv) > 2 else 60.0


def main():
    src = sys.argv[1]
    code = sys.stdin.read() if src == "-" else open(src).read()

    s = socket.create_connection((HOST, PORT), timeout=10)
    payload = json.dumps({"type": "execute", "code": code, "strict_json": False})
    s.sendall(payload.encode("utf-8") + b"\0")

    s.settimeout(TIMEOUT)
    t0 = time.time()
    buf = bytearray()
    while b"\0" not in buf:
        try:
            chunk = s.recv(65536)
        except socket.timeout:
            print(f"BX_TIMEOUT after {time.time()-t0:.1f}s, {len(buf)} bytes buffered", file=sys.stderr)
            sys.exit(2)
        if not chunk:
            print(f"BX_CLOSED after {time.time()-t0:.1f}s: {buf.decode(errors='replace')[:2000]}", file=sys.stderr)
            sys.exit(3)
        buf.extend(chunk)
    s.close()

    resp = json.loads(bytes(buf[: buf.index(b"\0")]).decode("utf-8"))
    elapsed = time.time() - t0
    status = resp.get("status")
    if resp.get("stdout"):
        print(resp["stdout"], end="")
    if resp.get("stderr"):
        print(resp["stderr"], end="", file=sys.stderr)
    if status == "ok":
        result = resp.get("result")
        if result:  # skip default empty dict
            print("RESULT:", json.dumps(result, indent=1))
        print(f"[bx ok {elapsed:.1f}s]", file=sys.stderr)
    else:
        print("BX_ERROR:", resp.get("message", resp), file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
