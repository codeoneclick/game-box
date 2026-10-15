"""Generate the native editor's small, shared low-poly icon atlas.

Each 96px tile has transparent padding so linear sampling stays within its tile.
The source PNG rows are inverted by the engine loader; UI atlas UVs account for it.
"""
import os
from PIL import Image, ImageDraw
import math

SIZE = 96
atlas = Image.new('RGBA', (SIZE * 8, SIZE * 2))
ivory = '#fff6dd'

def rounded_rectangle(d, box, radius, fill):
    x0,y0,x1,y1=box
    d.rectangle((x0+radius,y0,x1-radius,y1),fill=fill)
    d.rectangle((x0,y0+radius,x1,y1-radius),fill=fill)
    for x,y in [(x0,y0),(x1-radius*2,y0),(x0,y1-radius*2),(x1-radius*2,y1-radius*2)]:
        d.ellipse((x,y,x+radius*2,y+radius*2),fill=fill)

def icon(index):
    image = Image.new('RGBA', (SIZE, SIZE))
    return image, ImageDraw.Draw(image)

def tree(d, kind):
    d.polygon([(43,77),(48,29),(54,77),(64,84),(34,84)], fill='#ab6d36')
    if kind == 0:
        for y,w in [(14,12),(31,20),(49,28)]:
            d.polygon([(48,y),(48-w,y+30),(48+w,y+30)], fill='#75952a')
            d.polygon([(48,y),(48,y+30),(48-w,y+30)], fill='#aecb38')
    elif kind == 3:
        for dx,dy in [(-32,4),(-27,-17),(0,-25),(28,-14),(33,8)]:
            d.polygon([(48,30),(48+dx,30+dy),(48+int(dx*.9),47+dy),(48+int(dx*.45),36+int(dy*.5))], fill='#abc82c')
        d.polygon([(48,30),(52,64),(48,78),(45,32)],fill='#c68c44')
    else:
        if kind == 2:
            d.polygon([(45,79),(48,32),(52,79)], fill=ivory)
        colors = ['#608c2a','#a9c735','#87ad2e'] if kind == 1 else ['#b4b632','#e0d244','#c5c534']
        for x,y,r,c in [(30,41,18,colors[0]),(58,39,21,colors[2]),(45,23,19,colors[1]),(48,49,18,colors[1])]:
            pts=[(x+math.cos(a*math.pi/3)*r,y+math.sin(a*math.pi/3)*r) for a in range(6)]
            d.polygon(pts,fill=c)

def voxel_tree(d, kind):
    """Draw isolated Minecraft-style trees from cubes, without a ground tile.

    Tiles 8..11 are Pine/Oak/Birch/Palm; formerly empty tile 15 is Thuja.
    Keep the shared Trees tool glyph (tile 6) and other atlas entries intact.
    """
    cells = {}
    bark = (129, 85, 46)
    leaves = [(65, 91, 30), (85, 131, 42), (125, 153, 55),
              (74, 145, 55), (48, 111, 63)][kind]

    def block(x, y, z, color):
        cells[(x, y, z)] = color

    def crown(y0, y1, radius):
        for y in range(y0, y1 + 1):
            for x in range(-radius, radius + 1):
                for z in range(-radius, radius + 1):
                    # Small corner cuts retain a square, stepped silhouette.
                    if radius > 1 and abs(x) == radius and abs(z) == radius:
                        continue
                    variation = ((x * 13 + y * 7 + z * 17) % 5 - 2) * 4
                    block(x, y, z, tuple(c + variation for c in leaves))

    height = [14, 11, 14, 13, 16][kind]
    for y in range(height):
        color = bark
        if kind == 2:
            color = (67, 65, 54) if y % 3 == 1 else (224, 223, 202)
        block(0, y, 0, color)
    if kind == 0:
        # Match the new open boughs instead of the former pyramid plates.
        for tier, (base, radius) in enumerate([(4, 4), (7, 3), (10, 2), (12, 1), (14, 0)]):
            for x in range(-radius, radius + 1):
                for z in range(-radius, radius + 1):
                    dx, dz = x + .25 * (tier % 2), z - .25
                    distance = math.sqrt(dx * dx + dz * dz)
                    fan = .5 + .5 * math.cos(math.atan2(dz, dx) * 6 + tier * .83)
                    if radius and (distance > radius * (.78 + .22 * fan) + .3 or
                                   (distance > radius * .6 and fan < .16)):
                        continue
                    top = base + (1 if radius and distance < radius * .45 else 0)
                    tone = ((x * 13 + tier * 7 + z * 17) % 5 - 2) * 4
                    for y in range(top - 1, top + 1):
                        block(x, y, z, tuple(c + tone for c in leaves))
        for y in range(14):
            block(0, y, 0, bark)
    elif kind in (1, 2):
        for y0, y1, radius in ([(5, 7, 3), (8, 10, 2), (11, 11, 1)]
                               if kind == 1 else [(7, 10, 2), (11, 13, 1)]):
            crown(y0, y1, radius)
    elif kind == 3:
        crown(12, 12, 1)
        for dx, dz in [(1, 0), (-1, 0), (0, 1), (0, -1),
                       (1, 1), (-1, -1), (1, -1), (-1, 1)]:
            for step in range(1, 5):
                block(dx * step, 13 - max(0, step - 2), dz * step, leaves)
    else:
        for y0, y1, radius in [(2, 8, 2), (9, 12, 1), (13, 16, 0)]:
            crown(y0, y1, radius)

    for (x, y, z), color in sorted(cells.items(), key=lambda item: (item[0][0] + item[0][2], item[0][1])):
        sx, sy = 48 + (x - z) * 4, 82 + (x + z) * 2 - y * 4
        shade = lambda factor: tuple(min(255, int(c * factor)) for c in color)
        d.polygon([(sx, sy - 4), (sx + 4, sy - 2), (sx, sy), (sx - 4, sy - 2)], fill=shade(1.25))
        d.polygon([(sx - 4, sy - 2), (sx, sy), (sx, sy + 4), (sx - 4, sy + 2)], fill=shade(.85))
        d.polygon([(sx + 4, sy - 2), (sx, sy), (sx, sy + 4), (sx + 4, sy + 2)], fill=color)

for index in range(16):
    image,d = icon(index)
    if index in (0,1):
        d.polygon([(16,46),(42,24),(42,38),(62,38),(77,51),(77,75),(64,75),(64,55),(54,51),(42,51),(42,64)],fill=ivory)
        if index == 1: image = image.transpose(Image.FLIP_LEFT_RIGHT)
    elif index == 2:
        rounded_rectangle(d,(23,16,74,81),radius=5,fill=ivory)
        d.rectangle((34,18,64,40),fill='#194858');d.rectangle((40,19,58,34),fill=ivory)
        d.rectangle((34,52,63,72),fill='#194858')
    elif index == 3:
        rounded_rectangle(d,(17,32,81,78),radius=7,fill=ivory)
        rounded_rectangle(d,(17,23,49,49),radius=5,fill=ivory)
    elif index == 4:
        d.polygon([(14,64),(31,51),(65,49),(84,65),(77,78),(28,82),(12,72)],fill='#9b742e')
        d.polygon([(14,61),(31,49),(41,49),(47,35),(62,42),(66,51),(84,63),(62,72),(29,72)],fill='#bbd32d')
        d.polygon([(47,35),(47,67),(29,72),(14,61),(31,49),(41,49)],fill='#80a628')
        d.polygon([(48,4),(35,19),(43,19),(43,31),(53,31),(53,19),(61,19)],fill='#a1eeff')
        d.polygon([(76,34),(84,34),(84,43),(89,43),(80,54),(71,43),(76,43)],fill=ivory)
    elif index == 5:
        for pts,c in [([(13,80),(17,42),(33,59),(29,25),(49,58),(62,18),(58,60),(82,38),(72,80)],'#a8cf24'), ([(25,80),(38,50),(45,78),(51,38),(62,80)],'#c8de34')]: d.polygon(pts,fill=c)
    elif index == 6: tree(d,1)
    elif index == 7:
        d.ellipse((17,15,55,53),fill='#ffd52b')
        for a in range(8):
            ang=a*math.pi/4;d.line([(36+24*math.cos(ang),34+24*math.sin(ang)),(36+31*math.cos(ang),34+31*math.sin(ang))],fill='#ffd52b',width=3)
        for box in [(35,46,62,76),(53,38,77,69),(56,51,87,76)]:d.ellipse(box,fill='#59d2ef')
        d.rectangle((43,60,75,76),fill='#59d2ef')
    elif index < 12: voxel_tree(d,index-8)
    elif index == 12:
        # A baked soft halo stays one sprite draw in the UI.
        for width,alpha in [(13,18),(9,35),(6,65),(3,130),(1,255)]:
            points=[(48+39*math.cos(i*math.pi/64),48.5+28.5*math.sin(i*math.pi/64)) for i in range(129)]
            d.line(points,fill=(77,225,255,alpha),width=width)
    elif index in (13,14):
        points=[(58,20),(32,48),(58,76)] if index == 13 else [(38,20),(64,48),(38,76)]
        d.line(points,fill=ivory,width=7)
    else: voxel_tree(d,4)
    atlas.paste(image, ((index%8)*SIZE,(index//8)*SIZE))

# Biomes occupy a separate seven-tile row in the same texture.
full=Image.new('RGBA',(SIZE*8,SIZE*3));full.paste(atlas,(0,0))
biomes=[('#87a82f','#bccf48'),('#61762b','#9aac35'),('#cc7831','#e9a243'),('#50645b','#93a39c'),('#a4a398','#d5cbb3'),('#a8d7f6','#f1fbff'),('#e9b76a','#ffdda0')]
for i,(base,light) in enumerate(biomes):
    im=Image.new('RGBA',(SIZE,SIZE));d=ImageDraw.Draw(im)
    rounded_rectangle(d,(7,7,89,89),radius=10,fill=base)
    for j in range(8):
        x=10+(j*29)%71;y=12+(j*41)%64
        d.polygon([(x,y+14),(x+14,y),(x+29,y+12),(x+22,y+29)],fill=light)
    full.paste(im,(i*SIZE,2*SIZE))
output=os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), 'gbBundle', 'Textures', 'editor_ui_icons.png')
full.save(output)
print('Generated editor_ui_icons.png: %d x %d, 23 occupied tiles' % full.size)
