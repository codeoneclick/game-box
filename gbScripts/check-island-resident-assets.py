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
assert 1000 <= len(mesh.data.loop_triangles) <= 3000, len(mesh.data.loop_triangles)
assert abs(mesh.dimensions.z-1.9) < .001, mesh.dimensions.z
assert min(v.co.z for v in mesh.data.vertices) >= -.001
assert mesh.data.uv_layers.active is not None
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
