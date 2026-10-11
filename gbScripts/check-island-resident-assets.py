"""Run with Blender --background <blend> --python this-file -- --output-root <repo>."""
import bpy, math, os, sys, argparse
args = argparse.ArgumentParser()
args.add_argument('--output-root', default='.')
opts = args.parse_args(sys.argv[sys.argv.index('--')+1:] if '--' in sys.argv else [])
root = os.path.abspath(opts.output_root)
for name in ['island_resident.fbx', 'island_resident_idle.fbx', 'island_resident_walk.fbx']:
    assert os.path.isfile(os.path.join(root, 'gbBundle', 'Meshes', name)), 'Missing export: '+name
mesh = bpy.data.objects.get('IslandResident')
assert mesh is not None, 'Missing character'
rig = bpy.data.objects.get('ResidentRig')
assert rig is not None and 1 < len(rig.data.bones) <= 32
mesh.data.calc_loop_triangles()
assert 0 < len(mesh.data.loop_triangles) <= 500, 'Triangle budget exceeded: %d' % len(mesh.data.loop_triangles)
assert abs(mesh.dimensions.z-1.9) < .001, mesh.dimensions.z
assert abs(mesh.dimensions.x/mesh.dimensions.z-530.0/668) < .015, 'Blueprint arm span'
assert mesh.get('concept') == 'Low-Poly Character Turnaround Sheet (2).png'
# Check silhouette landmarks from the actual weighted geometry, not metadata.
head_group=mesh.vertex_groups['head'].index
head=[v.co for v in mesh.data.vertices if any(g.group==head_group for g in v.groups)]
assert .18 < (max(p.z for p in head)-min(p.z for p in head))/1.9 < .20, 'Blueprint head height'
assert .135 < (max(p.x for p in head)-min(p.x for p in head))/1.9 < .16, 'Blueprint head width'
for side in ['L','R']:
    foot=mesh.vertex_groups['foot.'+side].index
    sole=[v.co for v in mesh.data.vertices if any(g.group==foot and g.weight>.99 for g in v.groups) and v.co.z<.001]
    assert len(sole)>=4, 'Flat boot sole missing'
    assert max(p.y for p in sole)-min(p.y for p in sole)>.20, 'Blueprint boot toe'
    assert .18 < max(p.x for p in sole)-min(p.x for p in sole) < .24, 'Blueprint boot width'
assert min(v.co.z for v in mesh.data.vertices) >= -.001
assert mesh.data.uv_layers.active is not None
assert mesh.get('rest_pose') == 'T-pose'
for side in ['L', 'R']:
    for name in ['upper_arm.', 'forearm.', 'hand.']:
        bone = rig.data.bones[name+side]
        assert abs(bone.head_local.z-bone.tail_local.z) < .0001, 'Arms must be horizontal in T-pose'
for vertex in mesh.data.vertices:
    assert all(math.isfinite(x) for x in vertex.co)
    assert 1 <= len(vertex.groups) <= 4
    assert abs(sum(g.weight for g in vertex.groups)-1) < .001
for name in ['idle', 'walk']:
    assert bpy.data.actions.get(name) is not None, 'Missing action '+name
assert mesh.get('shirt_faces', 0) > 0 and mesh.get('trousers_faces', 0) > 0
for name in ['island_resident_palette.png', 'island_resident_clothing_mask.png']:
    assert os.path.isfile(os.path.join(root, 'gbBundle', 'Textures', name))
print('Resident asset checks passed: triangles=%d bones=%d height=%.3f' %
      (len(mesh.data.loop_triangles), len(rig.data.bones), mesh.dimensions.z))
