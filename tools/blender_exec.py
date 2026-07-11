"""Send Python code to Blender's official MCP extension socket and print the reply.

The extension (lab_blender_org/mcp) listens on localhost:9876 for
null-byte-delimited JSON: {"type": "execute", "code": ..., "strict_json": bool}.
The code runs inside Blender; assign a JSON-serializable dict to `result`.

Usage:
    python tools/blender_exec.py script.py     # send a file
    python tools/blender_exec.py -c "code"     # send a snippet
"""
import json
import socket
import sys

HOST, PORT = "localhost", 9876
TIMEOUT = 120  # mesh imports on large STLs can take a while


def run(code: str) -> dict:
    req = {"type": "execute", "code": code, "strict_json": False}
    s = socket.create_connection((HOST, PORT), timeout=10)
    s.settimeout(TIMEOUT)
    s.sendall(json.dumps(req).encode("utf-8") + b"\0")
    buf = bytearray()
    while b"\0" not in buf:
        chunk = s.recv(65536)
        if not chunk:
            break
        buf.extend(chunk)
    s.close()
    return json.loads(bytes(buf).rstrip(b"\0").decode("utf-8"))


def main() -> int:
    if len(sys.argv) >= 3 and sys.argv[1] == "-c":
        code = sys.argv[2]
    elif len(sys.argv) >= 2:
        with open(sys.argv[1], encoding="utf-8") as f:
            code = f.read()
    else:
        print(__doc__)
        return 2

    resp = run(code)
    if resp.get("stdout"):
        print(resp["stdout"], end="")
    if resp.get("stderr"):
        print(resp["stderr"], end="", file=sys.stderr)
    if resp.get("status") == "ok":
        print(json.dumps(resp.get("result", {}), indent=2))
        return 0
    print(resp.get("message", json.dumps(resp)), file=sys.stderr)
    return 1


if __name__ == "__main__":
    sys.exit(main())
