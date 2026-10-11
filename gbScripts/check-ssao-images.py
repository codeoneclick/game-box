"""Validate the fixed-time Vulkan captures produced by check-ssao-runtime.ps1."""
from __future__ import print_function
import os
from PIL import Image, ImageChops, ImageDraw

base = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'gbDemo',
                                   'output', 'island-check'))


def frame(case, number):
    image = Image.open(os.path.join(base, case, str(number) + '.ppm')).convert('RGB')
    # FPS changes between captures even when the scene and ocean time are fixed.
    # Exclude only the HUD from comparisons; keep all scene pixels below it.
    ImageDraw.Draw(image).rectangle((900, 0, 1279, 119), fill=(255, 255, 255))
    return image


on = frame('ssao-on', 160)
off = frame('ssao-off', 160)
mask = frame('ssao-mask', 160)
assert on.size == off.size == mask.size == (1280, 720), 'Unexpected capture size'
assert len(set(mask.getdata())) > 20, 'AO mask is constant or missing'
assert mask.crop((0, 0, 160, 40)).getextrema() == ((255, 255),) * 3, 'Background occludes'
red, green, blue = mask.split()
assert min(red.getdata()) < 245, 'No contact occlusion'
assert ImageChops.difference(red, green).getbbox() is None
assert ImageChops.difference(red, blue).getbbox() is None
delta = ImageChops.difference(on, off)
assert delta.crop((0, 380, 300, 700)).getbbox() is None, 'Open-water color changed'
assert ImageChops.subtract(on, off).getbbox() is None, 'AO unexpectedly brightens scene'
changed = sum(1 for p in delta.crop((360, 80, 800, 230)).getdata() if max(p) > 2)
assert changed > 500, 'SSAO does not affect terrace contacts'
for case in ('ssao-on', 'ssao-off', 'ssao-mask'):
    assert ImageChops.difference(frame(case, 80), frame(case, 160)).getbbox() is None, \
        'Fixed-scene AO flickers: ' + case
print('SSAO captures passed: contact darkening, neutral background, unchanged water, stable frames')
