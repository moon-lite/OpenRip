# Convert STEP/3MF reference parts to STL for Blender import.
# Run with: freecadcmd convert_to_stl.py
import os
import FreeCAD
import Mesh
import Part

REF_DIR = os.path.dirname(os.path.abspath(__file__))
OUT_DIR = os.path.join(REF_DIR, "stl")
os.makedirs(OUT_DIR, exist_ok=True)

JOBS = [
    ("Seeed Studio XIAO ESP32-C5.step", "xiao_esp32.stl"),
    ("SPDT slide switch SS12D00.step", "slide_switch_ss12d00.stl"),
    ("Omron_D2FC.3mf", "omron_d2fc.stl"),
]

for src_name, out_name in JOBS:
    src = os.path.join(REF_DIR, src_name)
    out = os.path.join(OUT_DIR, out_name)
    if not os.path.exists(src):
        print("MISSING:", src)
        continue
    try:
        if src.lower().endswith(".step"):
            shape = Part.Shape()
            shape.read(src)
            mesh = Mesh.Mesh()
            # LinearDeflection 0.05mm keeps small connector details crisp
            mesh.addFacets(shape.tessellate(0.05))
            mesh.write(out)
        else:  # 3mf is already a mesh
            mesh = Mesh.Mesh(src)
            mesh.write(out)
        bb = mesh.BoundBox
        print("OK: %s -> %s  bbox %.2f x %.2f x %.2f mm" % (
            src_name, out_name,
            bb.XLength, bb.YLength, bb.ZLength))
    except Exception as e:
        print("FAIL: %s  (%s)" % (src_name, e))
