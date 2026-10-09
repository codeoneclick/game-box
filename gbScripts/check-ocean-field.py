"""Compare full-map Vulkan captures against exact world-space seed distances.

Usage: python gbScripts/check-ocean-field.py MASK_DIR DISTANCE_DIR OTHER_CAMERA_DIR
Inputs are 160.ppm from GB_OCEAN_FIELD_VIEW=15/16 at identical fixed wave time.
Requires Pillow. Compatible with the workspace's Python 2 and Python 3.
"""
from __future__ import print_function
import math
import os
import sys
from PIL import Image, ImageChops


def check(mask_dir, distance_dir, other_camera_dir):
    mask = Image.open(os.path.join(mask_dir, "160.ppm")).convert("RGB")
    other = Image.open(os.path.join(other_camera_dir, "160.ppm")).convert("RGB")
    distance = Image.open(os.path.join(distance_dir, "160.ppm")).convert("RGB")
    assert mask.size == other.size == distance.size, "Capture size changed"
    assert ImageChops.difference(mask, other).getbbox() is None, "Camera changed the interaction mask"
    width, height = mask.size
    pixels = list(mask.getdata())
    solid = [p[0] > 127 for p in pixels]
    assert any(solid) and not all(solid), "Expected both occupied and water texels"
    seeds = []
    for y in range(height):
        for x in range(width):
            if not solid[y * width + x]:
                continue
            if x == 0 or y == 0 or x == width - 1 or y == height - 1 or any(
                    not solid[q] for q in (y * width + x - 1, y * width + x + 1,
                                          (y - 1) * width + x, (y + 1) * width + x)):
                seeds.append((x, y))
    dx, dz = 320.0 / width, 320.0 / height
    worst, checked = 0.0, 0
    for y in range(3, height, 11):
        for x in range(3, width, 11):
            if solid[y * width + x]:
                continue
            exact = max(math.sqrt(min(((x - sx) * dx) ** 2 + ((y - sy) * dz) ** 2
                                     for sx, sy in seeds)) - .15625, 0.0)
            # White is a clamped distance >= 8 m; compare unsaturated pixels.
            if exact >= 7.5:
                continue
            encoded = distance.getpixel((x, y))[0] / 255.0
            measured = encoded ** 2.2 * 8.0
            worst = max(worst, abs(exact - measured))
            checked += 1
    assert checked > 50, "Insufficient near-boundary samples"
    # Includes final 8-bit/gamma quantization and one rasterized map texel.
    assert worst <= max(dx, dz) + .12, "Distance error %.4f m" % worst
    print("Camera-independent mask; %d exact-distance samples, max error %.4f m" % (checked, worst))


if __name__ == "__main__":
    if len(sys.argv) != 4:
        raise SystemExit(__doc__)
    check(*sys.argv[1:])
