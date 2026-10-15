"""Author repeatable voxel terrain tiles and six linear-light filtered LODs.

No reference image is modified. The atlas layout is shared with the terrain
shader: seven 36-pixel columns, six 80-pixel LOD rows, top/side at y=1/41.
Requires Pillow; works with the repository's Python 2.7 and Python 3.
"""
from __future__ import print_function
from PIL import Image
import os
import random

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
NAMES = ['limestone', 'sand', 'shoreline', 'grass', 'lime_grass', 'dry_grass', 'mesa_clay']
PALETTES = [
    [(222,214,193),(205,196,176),(192,185,168),(234,226,206)],
    [(240,220,163),(227,202,143),(217,191,131),(248,232,181)],
    [(116,145,53),(100,132,45),(130,157,62),(142,166,70)],
    [(118,151,55),(132,165,64),(107,142,49),(147,177,75)],
    [(177,194,55),(192,209,66),(153,177,44),(165,186,51)],
    [(217,179,58),(230,194,74),(200,159,44),(239,205,84)],
    [(190,76,32),(213,89,37),(181,65,29),(227,103,43)]
]
SOILS = {
    2: [(194,163,105),(215,187,130),(174,144,91),(203,171,111)],
    3: [(133,84,43),(151,97,48),(118,73,39),(162,108,55)],
    4: [(183,148,96),(204,172,119),(161,128,81),(192,157,102)],
    5: [(153,91,37),(181,116,48),(136,77,31),(169,103,40)]
}

def tile(material, side):
    rng = random.Random(1337 + material * 101 + int(side) * 997)
    image = Image.new('RGBA', (32,32))
    # Four large color cells per 2m block; retain the atlas ABI and filtered LODs.
    variation = [[rng.randrange(4) for x in range(4)] for y in range(4)]
    fringe = [4 + 2*rng.randrange(3) for x in range(4)]
    shore = [[rng.random() < .32 for x in range(4)] for y in range(4)]
    clay = [(199,78,31)]*5 + [(158,58,28)]*5 + [(218,135,54)]*3 + [(237,188,132)]*3 + [(180,93,48)]*7 + [(163,70,33)]*9
    for y in range(32):
        for x in range(32):
            index = variation[y//8][x//8]
            palette = PALETTES[material]
            if material == 2 and not side and shore[y//8][x//8]:
                palette = PALETTES[1]
            if side and material in SOILS and y >= fringe[x//8]:
                palette = PALETTES[1] if material == 2 and y > 22 else SOILS[material]
            # Mineral ledge within the soil, with pixel-sized uneven boundaries.
            if side and material == 3 and x//8 != 2 and 10 + 2*(x//8)%6 <= y < 16 + 2*(x//8)%6:
                palette = [(132,137,120),(150,149,128),(119,128,113),(159,155,134)]
            rgb = palette[index]
            if side and material == 6:
                rgb = tuple(max(0,min(255,c + [-8,0,5,11][index])) for c in clay[y])
            image.putpixel((x,y),rgb+(255,))
    return image

def linear(c):
    c = c / 255.0
    return c / 12.92 if c <= .04045 else ((c+.055)/1.055)**2.4

def srgb(c):
    return int(round(255 * (12.92*c if c <= .0031308 else 1.055*c**(1/2.4)-.055)))

def downsample(source, size):
    output = Image.new('RGBA',(size,size))
    step = 32//size
    for y in range(size):
        for x in range(size):
            values=[source.getpixel((x*step+dx,y*step+dy)) for dy in range(step) for dx in range(step)]
            color=tuple(srgb(sum(linear(v[c]) for v in values)/len(values)) for c in range(3))
            output.putpixel((x,y),color+(255,))
    return output

def generate():
    atlas=Image.new('RGBA',(256,480),(0,0,0,255))
    for material in range(7):
        for side in (False,True):
            source=tile(material,side)
            for lod in range(6):
                size=32>>lod
                image=source if lod==0 else downsample(source,size)
                origin_x=material*36+2
                origin_y=lod*80+(40 if side else 0)+1
                for y in range(-1,size+1):
                    for x in range(-2,size+2):
                        atlas.putpixel((origin_x+x,origin_y+y),image.getpixel((x%size,y%size)))
    path=os.path.join(ROOT,'gbBundle','Textures','terrain_voxel_atlas.png')
    atlas.save(path)
    print('Generated terrain_voxel_atlas.png: 256x480 RGBA, 7 materials, top/side, 6 LODs')

if __name__ == '__main__':
    generate()

