"""Export immutable Blockbench buildings and render their UI thumbnails.

Blender --background --python-exit-code 1 --python gbScripts/export-buildings.py -- --root .
Reports/roundtrip references go to ignored gbDemo/output/island-check/buildings.
Blockbench references: js/outliner/types/cube.js::UVToLocal,
js/outliner/outliner.js::NodePreviewController.updateTransform,
js/io/format.ts (default Three.js Euler order ZYX).
"""
import argparse
import base64
import hashlib
import json
import math
import os
import struct
import sys
import zlib

import bpy
from mathutils import Matrix, Vector

BUILDINGS = [('forester_hut', 'Forester', 431, 12, 1, 'building_forester_atlas.png'),
             ('stonecutters_hut', 'Stonecutter', 336, 10, 1, 'building_workers_atlas.png'),
             ('woodcutter_hut', 'Woodcutter', 310, 9, 2, 'building_workers_atlas.png')]
SCALE = 1.0 / 16.0


def digest(data):
    return hashlib.sha256(data).hexdigest()


def write_emissive_mask(root):
    """Author-space PNG rows, flipped by the engine to match FBX V=1-y/height.

    RGBA grayscale matches the engine's Vulkan R8G8B8A8 texture upload path.
    Mask membership comes only from named emissive faces, never texel colors.
    """
    common = None
    source_hashes = {}
    for stem, *_ in BUILDINGS:
        path = os.path.join(root, 'gbResources', stem + '.bbmodel')
        with open(path, 'rb') as stream: original = stream.read()
        source_hashes[stem] = digest(original)
        model = json.loads(original.decode('utf-8-sig'))
        size = (model['resolution']['width'], model['resolution']['height'])
        lights = [e for e in model['elements'] if e['name'] == 'lantern_glow_box']
        assert len(lights) == 1 and lights[0].get('export', True)
        rects = []
        for face in lights[0]['faces'].values():
            assert face.get('texture') == 0 and face.get('enabled', True)
            u0, v0, u1, v1 = face['uv']
            rect = (min(u0, u1), min(v0, v1), max(u0, u1), max(v0, v1))
            assert all(int(v) == v for v in rect)
            rects.append(tuple(int(v) for v in rect))
        assert len(rects) == 6 and len(set(rects)) == 1
        rect = rects[0]
        assert size == (256, 256) and rect == (200, 128, 208, 136)
        if common is None: common = (size, rect)
        assert common == (size, rect)
        for element in model['elements']:
            if element['uuid'] == lights[0]['uuid']: continue
            for face in element['faces'].values():
                if face.get('texture') is None or not face.get('enabled', True): continue
                u0, v0, u1, v1 = face['uv']
                # Shared rectangle boundaries have zero UV area. NEAREST prevents
                # filtering the white mask texels into adjacent casing regions.
                overlaps = (max(min(u0, u1), rect[0]) < min(max(u0, u1), rect[2]) and
                            max(min(v0, v1), rect[1]) < min(max(v0, v1), rect[3]))
                assert not overlaps, 'Emissive atlas rectangle reused by ' + element['name']
        with open(path, 'rb') as stream: assert stream.read() == original
    (width, height), (x0, y0, x1, y1) = common
    rows = []
    for y in range(height):
        row = bytearray(b'\x00\x00\x00\xff' * width)
        if y0 <= y < y1:
            row[x0*4:x1*4] = b'\xff' * ((x1-x0)*4)
        rows.append(b'\x00' + bytes(row))
    def chunk(kind, payload):
        return struct.pack('>I', len(payload)) + kind + payload + struct.pack('>I', zlib.crc32(kind+payload) & 0xffffffff)
    png = (b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', width,height,8,6,0,0,0)) +
           chunk(b'IDAT', zlib.compress(b''.join(rows), 9)) + chunk(b'IEND', b''))
    path = os.path.join(root, 'gbBundle', 'Textures', 'building_emissive_mask.png')
    with open(path, 'wb') as stream: stream.write(png)
    report = {'filename':'building_emissive_mask.png', 'size':[width,height],
              'author_png_rectangle':[x0,y0,x1,y1], 'white_pixels':(x1-x0)*(y1-y0),
              'gpu_uv_rectangle':[x0/width,1-y1/height,x1/width,1-y0/height],
              'sha256':digest(png), 'source_sha256':source_hashes, 'png_format':'RGBA grayscale, opaque alpha',
              'classification':'six lantern_glow_box faces per building; no other UV area overlap'}
    report_dir = os.path.join(root,'gbDemo','output','island-check','buildings')
    os.makedirs(report_dir, exist_ok=True)
    with open(os.path.join(report_dir,'emissive-mask.json'),'w') as stream: json.dump(report,stream,indent=2)
    print('BUILDING_EMISSIVE_MASK ' + json.dumps(report), flush=True)


def rotation(node):
    # Three.js ZYX is Rz * Ry * Rx, with column vectors.
    x, y, z = [math.radians(a) for a in node.get('rotation', [0, 0, 0])]
    return Matrix.Rotation(z, 4, 'Z') @ Matrix.Rotation(y, 4, 'Y') @ Matrix.Rotation(x, 4, 'X')


def pivot_transform(node):
    pivot = Vector(node.get('origin', [0, 0, 0]))
    return Matrix.Translation(pivot) @ rotation(node) @ Matrix.Translation(-pivot)


def uv_corner(a, b, side, u, v, angle):
    for _ in range(angle // 90):
        u, v = 1 - v, u
    if side == 'north': return (b[0] + (a[0]-b[0])*u, b[1] + (a[1]-b[1])*v, a[2])
    if side == 'south': return (a[0] + (b[0]-a[0])*u, b[1] + (a[1]-b[1])*v, b[2])
    if side == 'east': return (b[0], b[1] + (a[1]-b[1])*v, b[2] + (a[2]-b[2])*u)
    if side == 'west': return (a[0], b[1] + (a[1]-b[1])*v, a[2] + (b[2]-a[2])*u)
    if side == 'up': return (a[0] + (b[0]-a[0])*u, b[1], a[2] + (b[2]-a[2])*v)
    if side == 'down': return (a[0] + (b[0]-a[0])*u, a[1], b[2] + (a[2]-b[2])*v)
    raise ValueError(side)


def check_conventions():
    # Independent non-commuting reference: Rx then Rz maps +Y to +Z.
    # Reversing these rotations would instead map it to -X.
    value = rotation({'rotation':[90,0,90]}) @ Vector((0,1,0,0))
    assert (value-Vector((0,0,1,0))).length < 1e-6
    element = {'origin':[1,2,3],'rotation':[90,0,0]}
    group = {'origin':[4,5,6],'rotation':[0,0,90]}
    value = pivot_transform(group) @ pivot_transform(element) @ Vector((1,3,3))
    assert (value-Vector((7,2,4))).length < 1e-6
    # UVToLocal rotates the normalized coordinate before choosing the face.
    a, b = [0,0,0], [2,4,6]
    assert uv_corner(a,b,'north',0,0,90) == (0,4,0)
    assert uv_corner(a,b,'east',0,0,180) == (2,0,0)
    assert uv_corner(a,b,'up',0,0,270) == (0,4,6)
    for side in ('north','south','east','west','up','down'):
        base = {uv_corner(a,b,side,u,v,0) for u,v in ((0,0),(1,0),(1,1),(0,1))}
        for angle in (90,180,270):
            assert {uv_corner(a,b,side,u,v,angle) for u,v in ((0,0),(1,0),(1,1),(0,1))} == base


def geometry(model):
    groups = {g['uuid']: g for g in model['groups']}
    transforms = {}
    def walk(nodes, parent=Matrix.Identity(4)):
        for node in nodes:
            if isinstance(node, str):
                assert node not in transforms
                transforms[node] = parent
            else:
                walk(node.get('children', []), parent @ pivot_transform(groups[node['uuid']]))
    walk(model['outliner'])
    assert len(transforms) == len(model['elements'])
    verts, faces, uvs = [], [], []
    normals = {'north': (0,0,-1), 'south': (0,0,1), 'east': (1,0,0),
               'west': (-1,0,0), 'up': (0,1,0), 'down': (0,-1,0)}
    def blender(v): return Vector((-v[0]*SCALE, v[2]*SCALE, v[1]*SCALE))
    uv_rotations = 0
    for element in model['elements']:
        assert element['type'] == 'cube' and element.get('export', True)
        assert not element.get('inflate', 0) and not element.get('rescale', False)
        assert element.get('stretch', [1,1,1]) == [1,1,1]
        a, b = element['from'], element['to']
        transform = transforms[element['uuid']] @ pivot_transform(element)
        lookup = {}
        for x in (a[0], b[0]):
            for y in (a[1], b[1]):
                for z in (a[2], b[2]):
                    lookup[(x,y,z)] = len(verts)
                    verts.append(blender(transform @ Vector((x,y,z))))
        for side, face in element['faces'].items():
            if face.get('texture') is None or not face.get('enabled', True): continue
            assert face['texture'] == 0
            angle = int(face.get('rotation', 0)); assert angle in (0,90,180,270)
            uv_rotations += int(angle != 0)
            indices, coords = [], []
            rect = face['uv']
            for u, v in ((0,0),(1,0),(1,1),(0,1)):
                indices.append(lookup[uv_corner(a,b,side,u,v,angle)])
                coords.append(((rect[0]+(rect[2]-rect[0])*u)/model['resolution']['width'],
                               1-(rect[1]+(rect[3]-rect[1])*v)/model['resolution']['height']))
            n = (verts[indices[1]]-verts[indices[0]]).cross(verts[indices[2]]-verts[indices[0]])
            if n.dot(blender(transform.to_3x3() @ Vector(normals[side]))) < 0:
                indices.reverse(); coords.reverse()
            faces.append(indices); uvs.append(coords)
    lo = Vector([min(v[i] for v in verts) for i in range(3)])
    hi = Vector([max(v[i] for v in verts) for i in range(3)])
    center = Vector(((lo.x+hi.x)/2, (lo.y+hi.y)/2, lo.z))
    verts = [v-center for v in verts]
    return verts, faces, uvs, (hi.x-lo.x)/2, (hi.y-lo.y)/2, hi.z-lo.z, uv_rotations


def render_preview(scene, mesh, height, path):
    world = bpy.data.worlds.new('BuildingPreviewWorld'); scene.world = world; world.use_nodes = True
    world.node_tree.nodes['Background'].inputs[0].default_value = (.45,.45,.45,1)
    world.node_tree.nodes['Background'].inputs[1].default_value = .6
    scene.render.engine = 'BLENDER_EEVEE'
    scene.render.resolution_x = scene.render.resolution_y = 512
    scene.render.resolution_percentage = 100
    scene.render.film_transparent = True
    scene.render.image_settings.file_format = 'PNG'; scene.render.image_settings.color_mode = 'RGBA'
    scene.view_settings.view_transform = 'Standard'
    scene.view_settings.look = 'Medium High Contrast'
    target = Vector((0,0,height*.46))
    camera_data = bpy.data.cameras.new('PreviewCamera')
    camera = bpy.data.objects.new('PreviewCamera', camera_data); scene.collection.objects.link(camera)
    camera.location = (12,-18,13)
    camera.rotation_euler = (target-camera.location).to_track_quat('-Z','Y').to_euler()
    camera_data.type = 'ORTHO'; scene.camera = camera
    inverse = camera.rotation_euler.to_matrix().transposed()
    projected = [inverse @ (v.co-target) for v in mesh.data.vertices]
    camera_data.ortho_scale = max(max(v.x for v in projected)-min(v.x for v in projected),
                                  max(v.y for v in projected)-min(v.y for v in projected))*1.14
    for name, pos, energy, size in [('Key', (0,-8,14), 2200, 10), ('Fill', (-9,2,8), 1400, 12)]:
        data = bpy.data.lights.new(name,'AREA'); data.energy = energy; data.shape = 'DISK'; data.size = size
        light = bpy.data.objects.new(name,data); scene.collection.objects.link(light); light.location = pos
        light.rotation_euler = (target-light.location).to_track_quat('-Z','Y').to_euler()
    scene.render.filepath = path; bpy.ops.render.render(write_still=True)
    preview = bpy.data.images.load(path, check_existing=False)
    assert tuple(preview.size) == (512,512) and preview.channels == 4
    alpha = list(preview.pixels)[3::4]
    assert min(alpha) == 0 and max(alpha) > .99 and sum(a>.1 for a in alpha) > 15000


def main():
    parser = argparse.ArgumentParser(); parser.add_argument('--root', default='.')
    parser.add_argument('--mask-only', action='store_true', help='Regenerate the emissive PNG without touching meshes or previews')
    args = parser.parse_args(sys.argv[sys.argv.index('--')+1:] if '--' in sys.argv else [])
    check_conventions()
    root = os.path.abspath(args.root)
    write_emissive_mask(root)
    if args.mask_only: return
    mesh_dir = os.path.join(root,'gbBundle','Meshes'); tex_dir = os.path.join(root,'gbBundle','Textures')
    report_dir = os.path.join(root,'gbDemo','output','island-check','buildings'); os.makedirs(report_dir,exist_ok=True)
    reports, definitions, textures = [], [], {}
    # Generate the catalog before slower thumbnail rendering so dependent C++ can build.
    models = []
    for stem, name, cubes, elements_rotated, groups_rotated, texture_name in BUILDINGS:
        source = os.path.join(root,'gbResources',stem+'.bbmodel')
        with open(source,'rb') as stream: original = stream.read()
        model = json.loads(original.decode('utf-8-sig'))
        assert len(model['elements']) == cubes and len(model['textures']) == 1
        assert sum(any(e.get('rotation',[])) for e in model['elements']) == elements_rotated
        assert sum(any(g.get('rotation',[])) for g in model['groups']) == groups_rotated
        geo = geometry(model); verts, faces, uvs, hw, hd, height, uv_rotations = geo
        definitions.append('        {"%s", "%s.fbx", "%s", "%s_preview.png", %.9ff, %.9ff}' % (name,stem,texture_name,stem,hw,hd))
        models.append((stem, source, original, model, geo, texture_name, elements_rotated, groups_rotated))
    header = '''// Generated by gbScripts/export-buildings.py; bounds include the entire model.
#pragma once
#include <cassert>
#include <cstddef>
namespace game
{
    enum class BuildingType { Forester = 0, Stonecutter = 1, Woodcutter = 2 };
    struct BuildingDefinition
    {
        const char* name;
        const char* mesh_filename;
        const char* texture_filename;
        const char* preview_filename;
        float half_width;
        float half_depth;
    };
    inline const BuildingDefinition& building_definition(BuildingType type)
    {
        static const BuildingDefinition definitions[] = {
%s
        };
        const auto index = static_cast<std::size_t>(type);
        assert(index < 3);
        return definitions[index < 3 ? index : 0];
    }
}
''' % ',\n'.join(definitions)
    with open(os.path.join(root,'gbDemo','sources','building_catalog.h'),'w',newline='\n') as stream: stream.write(header)
    for stem, source, original, model, geo, texture_name, er, gr in models:
        bpy.ops.object.select_all(action='SELECT'); bpy.ops.object.delete(use_global=False)
        scene = bpy.context.scene; scene.unit_settings.system = 'METRIC'; scene.unit_settings.scale_length = 1
        verts, faces, uvs, hw, hd, height, uv_rotations = geo
        data = bpy.data.meshes.new(stem); data.from_pydata(verts,[],faces); data.update()
        mesh = bpy.data.objects.new(stem,data); scene.collection.objects.link(mesh)
        layer = data.uv_layers.new(name='BlockbenchUV')
        for polygon, coords in zip(data.polygons,uvs):
            for loop, uv in zip(polygon.loop_indices,coords): layer.data[loop].uv = uv
        data.calc_loop_triangles(); assert len(data.loop_triangles) == len(faces)*2
        assert len(verts) == len(model['elements'])*8
        png = base64.b64decode(model['textures'][0]['source'].split(',',1)[1])
        if texture_name in textures: assert textures[texture_name] == digest(png)
        textures[texture_name] = digest(png)
        texture_path = os.path.join(tex_dir,texture_name)
        with open(texture_path,'wb') as stream: stream.write(png)
        image = bpy.data.images.load(texture_path,check_existing=False); assert tuple(image.size) == (256,256)
        material = bpy.data.materials.new(stem+'_original'); material.use_nodes = True
        tex = material.node_tree.nodes.new('ShaderNodeTexImage'); tex.image = image; tex.interpolation = 'Closest'
        bsdf = material.node_tree.nodes.get('Principled BSDF'); bsdf.inputs['Roughness'].default_value = .9
        material.node_tree.links.new(tex.outputs['Color'],bsdf.inputs['Base Color'])
        material.node_tree.links.new(tex.outputs['Alpha'],bsdf.inputs['Alpha'])
        data.materials.append(material)
        bpy.ops.object.select_all(action='DESELECT'); mesh.select_set(True); bpy.context.view_layer.objects.active = mesh
        fbx = os.path.join(mesh_dir,stem+'.fbx')
        bpy.ops.export_scene.fbx(filepath=fbx,use_selection=True,object_types={'MESH'},axis_forward='-Z',axis_up='Y',
                                 apply_scale_options='FBX_SCALE_ALL',bake_anim=False,use_mesh_modifiers=False,path_mode='STRIP')
        # Roundtrip checks preserve positions and UV seam corners; final ufbx loader check is separate.
        reference = sorted(tuple(round(c,5) for c in tuple(verts[index])+tuple(uv))
                           for face,coords in zip(faces,uvs) for index,uv in zip(face,coords))
        with open(os.path.join(report_dir,stem+'_expected.json'),'w') as stream:
            json.dump({'vertices_game':[[v.x,v.z,-v.y] for v in verts], 'blender_uv_corners':reference},stream)
        with open(os.path.join(report_dir,stem+'_expected_vertices.txt'),'w') as stream:
            for v in verts: stream.write('%.10f %.10f %.10f\n' % (v.x,v.z,-v.y))
        with open(os.path.join(report_dir,stem+'_expected_uv.txt'),'w') as stream:
            for face, coords in zip(faces,uvs):
                for index, uv in zip(face,coords):
                    v = verts[index]
                    stream.write('%.10f %.10f %.10f %.10f %.10f\n' % (v.x,v.z,-v.y,uv[0],uv[1]))
        bpy.ops.import_scene.fbx(filepath=fbx)
        imported = [o for o in bpy.context.selected_objects if o.type=='MESH']; assert len(imported)==1
        other = imported[0]; actual = sorted(tuple(round(c,5) for c in tuple(other.matrix_world @ other.data.vertices[other.data.loops[loop].vertex_index].co)+tuple(other.data.uv_layers.active.data[loop].uv))
                                                 for p in other.data.polygons for loop in p.loop_indices)
        assert len(actual)==len(reference) and all(max(abs(a-b) for a,b in zip(x,y))<.00003 for x,y in zip(actual,reference))
        assert not other.modifiers and not other.animation_data and not any(o.type=='ARMATURE' for o in bpy.context.selected_objects)
        bpy.data.objects.remove(other,do_unlink=True)
        render_preview(scene,mesh,height,os.path.join(tex_dir,stem+'_preview.png'))
        with open(source,'rb') as stream: assert stream.read()==original
        report = {'name':stem,'source_sha256':digest(original),'texture_sha256':digest(png),'cubes':len(model['elements']),
                  'rotated_elements':er,'rotated_groups':gr,'rotated_uv_faces':uv_rotations,'vertices':len(verts),
                  'faces':len(faces),'triangles':len(data.loop_triangles),'blockbench_unit_m':SCALE,
                  'half_width':hw,'half_depth':hd,'height_m':height,'euler_order':'ZYX','static':True,
                  'fbx_roundtrip_geometry_uv':'pass','source_unchanged':True,'pivot':'full XZ bounds center, minimum Y=0'}
        reports.append(report); print('BUILDING_EXPORT '+json.dumps(report),flush=True)
    assert len(set(textures.values()))==2
    with open(os.path.join(report_dir,'export.json'),'w') as stream: json.dump(reports,stream,indent=2)


if __name__ == '__main__': main()
