"""Validate and render the saved Blockbench asset, never a substitute reference.

Blender --background --python-exit-code 1 --python gbScripts/render-party-liner.py -- .
Append --validate-only after the root to check geometry without rendering.
Uses the repository's verified Blockbench UV/ZYX pivot conventions.
"""
import base64
import hashlib
import importlib.util
import json
import math
from pathlib import Path
import sys

import bpy
from mathutils import Matrix, Vector

sys.dont_write_bytecode = True
ROOT = Path(sys.argv[sys.argv.index('--') + 1] if '--' in sys.argv else '.').resolve()
OUT = ROOT / 'gbResources/rhythm_party_liner'
source = ROOT / 'gbResources/rhythm_party_liner.bbmodel'
model = json.loads(source.read_text(encoding='utf-8-sig'))
report = json.loads((OUT / 'authoring-report.json').read_text())
spec = importlib.util.spec_from_file_location('bb_conventions', ROOT / 'gbScripts/export-buildings.py')
conventions = importlib.util.module_from_spec(spec)
spec.loader.exec_module(conventions)
conventions.check_conventions()

assert model['meta']['model_format'] == 'free'
assert len(model['textures']) == 1
assert base64.b64decode(model['textures'][0]['source'].split(',')[1]) == (OUT / 'palette.png').read_bytes()
assert model['resolution'] == {'width': 128, 'height': 128}
groups = {g['uuid']: g for g in model['groups']}
elements = {e['uuid']: e for e in model['elements']}
assert len(elements) == len(model['elements']), 'Duplicate element UUID'
owners, transforms, paths = {}, {}, {}

def walk(nodes, parent=Matrix.Identity(4), names=()):
    for node in nodes:
        if isinstance(node, str):
            assert node in elements and node not in owners
            owners[node] = names[-1]
            paths[node] = names
            transforms[node] = parent
        else:
            g = groups[node['uuid']]
            walk(node.get('children', []), parent @ conventions.pivot_transform(g), names + (g['name'],))

walk(model['outliner'])
assert set(owners) == set(elements), 'Orphan element'
required = ['hull', 'decks', 'superstructure', 'funnels', 'lifeboats', 'railings', 'lamps',
            'dance_floor', 'stage', 'dj_console', 'speakers', 'screen', 'stairs', 'props']
assert all(any(g['name'] == name for g in groups.values()) for name in required)
assert sum(g['name'].startswith('funnel_') for g in groups.values()) == 3
stage_top = next(e for e in elements.values() if e['name'].startswith('stage_teak_'))
for side in (-1, 1):
    treads = [e for e in elements.values() if e['name'].startswith('stage_stair_')
              and (e['from'][0] + e['to'][0]) * side > 0]
    top = max(treads, key=lambda e: e['to'][1])
    assert abs(top['to'][1] - stage_top['to'][1]) < .01, 'Stage stair height mismatch'
    assert (top['from'][0] < stage_top['to'][0] and top['to'][0] > stage_top['from'][0]
            and top['from'][2] < stage_top['to'][2] and top['to'][2] > stage_top['from'][2]), 'Disconnected stage stairs'

bpy.ops.object.select_all(action='SELECT')
bpy.ops.object.delete(use_global=False)
scene = bpy.context.scene
scene.render.engine = 'CYCLES'
scene.cycles.samples = 8
scene.cycles.use_denoising = True
scene.cycles.device = 'CPU'
scene.render.resolution_percentage = 100
scene.render.image_settings.file_format = 'PNG'
scene.view_settings.view_transform = 'AgX'
scene.render.film_transparent = False
scene.world.color = (.13, .16, .23)
scene.world.use_nodes = True
scene.world.node_tree.nodes['Background'].inputs['Color'].default_value = (.19, .25, .40, 1)
scene.world.node_tree.nodes['Background'].inputs['Strength'].default_value = .35

image = bpy.data.images.load(str(OUT / 'palette.png'))
image.pack()
materials = []
for name, emission in [('static', 0), ('warm_windows', 1.8), ('festoon_bulbs', 3.0),
                       ('dance_tiles', .7), ('stage_neon', 2.2), ('console_leds', 2.0)]:
    material = bpy.data.materials.new(name)
    material.use_nodes = True
    material.diffuse_color = (.4, .4, .4, 1)
    tree = material.node_tree
    shader = tree.nodes.get('Principled BSDF')
    shader.inputs['Roughness'].default_value = .62
    texture = tree.nodes.new('ShaderNodeTexImage')
    texture.image = image
    texture.interpolation = 'Closest'
    tree.links.new(texture.outputs['Color'], shader.inputs['Base Color'])
    if emission:
        tree.links.new(texture.outputs['Color'], shader.inputs['Emission Color'])
        shader.inputs['Emission Strength'].default_value = emission
    materials.append(material)

emissive_names = [m.name for m in materials]
batches = {}
lo, hi = Vector((math.inf,) * 3), Vector((-math.inf,) * 3)
obstacles = []
triangles = 0
normals = {'north': (0,0,-1), 'south': (0,0,1), 'east': (1,0,0),
           'west': (-1,0,0), 'up': (0,1,0), 'down': (0,-1,0)}
convert = Matrix(((1,0,0,0), (0,0,-1,0), (0,1,0,0), (0,0,0,1)))
for uuid, element in elements.items():
    assert element.get('type') == 'cube' and element.get('export', True)
    a, b = element['from'], element['to']
    assert all(math.isfinite(v) for field in ['from','to','origin','rotation'] for v in element.get(field, []))
    assert all(a[i] < b[i] for i in range(3))
    transform = transforms[uuid] @ conventions.pivot_transform(element)
    corners = [transform @ Vector((x,y,z)) for x in (a[0],b[0]) for y in (a[1],b[1]) for z in (a[2],b[2])]
    elo = Vector(tuple(min(v[i] for v in corners) for i in range(3)))
    ehi = Vector(tuple(max(v[i] for v in corners) for i in range(3)))
    for i in range(3):
        lo[i] = min(lo[i], elo[i]); hi[i] = max(hi[i], ehi[i])
    # Headroom for the game's 3.2-unit characters across the actual play rectangle.
    if (elo.x < 17.9 and ehi.x > -17.9 and elo.z < 27.9 and ehi.z > -19.9 and
            elo.y < 7.15 and ehi.y > 3.88):
        obstacles.append(element['name'])
    batch = batches.setdefault(owners[uuid], {'vertices': [], 'faces': [], 'uvs': [], 'materials': []})
    for side, face in element['faces'].items():
        if face.get('texture') is None or not face.get('enabled', True): continue
        assert face['texture'] == 0
        assert all(math.isfinite(v) and 0 <= v <= 128 for v in face['uv'])
        ids, coords = [], []
        for u, v in [(0,0), (1,0), (1,1), (0,1)]:
            point = conventions.uv_corner(a,b,side,u,v,int(face.get('rotation',0)))
            ids.append(len(batch['vertices']))
            batch['vertices'].append(convert @ transform @ Vector(point))
            r = face['uv']
            coords.append(((r[0] + (r[2]-r[0])*u)/128, 1-(r[1]+(r[3]-r[1])*v)/128))
        normal = (batch['vertices'][ids[1]]-batch['vertices'][ids[0]]).cross(batch['vertices'][ids[2]]-batch['vertices'][ids[0]])
        if normal.dot(convert.to_3x3() @ transform.to_3x3() @ Vector(normals[side])) < 0:
            ids.reverse(); coords.reverse()
        batch['faces'].append(ids); batch['uvs'].append(coords)
        batch['materials'].append(emissive_names.index(owners[uuid]) if owners[uuid] in emissive_names else 0)
        triangles += 2

assert not obstacles, 'Blocked dance area: '+str(obstacles)
assert 15000 <= triangles <= 35000 and triangles == report['triangles']
assert all(abs(lo[i]-report['bounds']['min'][i]) < .001 and abs(hi[i]-report['bounds']['max'][i]) < .001 for i in range(3)), 'Native Blockbench versus converted bounds mismatch'
for name, batch in batches.items():
    mesh = bpy.data.meshes.new(name)
    mesh.from_pydata(batch['vertices'], [], batch['faces']); mesh.update()
    obj = bpy.data.objects.new(name, mesh); scene.collection.objects.link(obj)
    for material in materials: mesh.materials.append(material)
    layer = mesh.uv_layers.new(name='BlockbenchUV')
    for face, coords, material in zip(mesh.polygons, batch['uvs'], batch['materials']):
        face.material_index = material
        for index, uv in zip(face.loop_indices, coords): layer.data[index].uv = uv
        face.use_smooth = False

def light(name, kind, location, energy, color, size=50, target=(0,50,0)):
    data = bpy.data.lights.new(name, kind)
    data.energy = energy; data.color = color
    if kind == 'AREA': data.shape = 'DISK'; data.size = size
    obj = bpy.data.objects.new(name, data);scene.collection.objects.link(obj)
    obj.location = location;obj.rotation_euler = (Vector(target)-obj.location).to_track_quat('-Z','Y').to_euler()
    return obj

sun = light('Warm sunset key', 'SUN', (120,-70,130), 2.6, (1,.71,.43))
sun.data.angle = .12
light('Cool sky fill', 'AREA', (-80,-20,110), 70000, (.47,.67,1), 100)
light('Stage fill', 'AREA', (0,-5,30), 5000, (.85,.4,.8), 30, (0,30,8))

# The blue plane is only a preview backdrop; it is excluded from the asset model.
bpy.ops.mesh.primitive_plane_add(size=2000, location=(0,0,-13.5))
water = bpy.context.object; water.name = 'PREVIEW_ONLY_ocean'
water_material = bpy.data.materials.new('PREVIEW_ONLY_ocean')
water_material.use_nodes = True
shader = water_material.node_tree.nodes['Principled BSDF']
shader.inputs['Base Color'].default_value = (.018,.067,.12,1)
shader.inputs['Roughness'].default_value = .32
shader.inputs['Metallic'].default_value = .2
water.data.materials.append(water_material)
camera_data = bpy.data.cameras.new('Asset camera')
camera = bpy.data.objects.new('Asset camera', camera_data);scene.collection.objects.link(camera)
scene.camera = camera;camera_data.type = 'ORTHO';camera_data.clip_end = 3000

def render(name, position, target, scale, width, height):
    camera.location = position
    camera.rotation_euler = (Vector(target)-camera.location).to_track_quat('-Z','Y').to_euler()
    camera_data.ortho_scale = scale
    scene.render.resolution_x = width;scene.render.resolution_y = height
    scene.render.filepath = str(OUT / name)
    bpy.ops.render.render(write_still=True)

validation = {'source_sha256': hashlib.sha256(source.read_bytes()).hexdigest(),
              'cubes': len(elements), 'triangles': triangles, 'groups': len(groups),
              'atlas': [128,128], 'native_bounds_match': True,
              'stage_stairs_connected': True,
              'bounds': {'min': list(lo), 'max': list(hi)},
              'dance_clearance': {'obstacles': obstacles, 'x': [-17.9,17.9], 'z': [-19.9,27.9],
                                  'clear_y': [3.88,7.15]},
              'preview_only': 'Sun, fill lights and blue ocean plane are not part of the bbmodel.'}
(OUT / 'validation.json').write_text(json.dumps(validation,indent=2)+'\n')
if '--validate-only' not in sys.argv:
    render('overview.png', (160,-185,123), (0,48,9), 225, 1600, 1050)
    render('party-deck.png', (70,-100,78), (0,11,9), 100, 1400, 1200)
    render('dj-console.png', (23,3,25), (0,31,10), 43, 1400, 900)
print('PARTY_LINER_VALIDATED '+json.dumps(validation), flush=True)
