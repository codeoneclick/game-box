"""Blender 4.3: export Blockbench hand and its native sampled Idle to Game Box FBX.
The authoring script bakes native Blockbench matrices; no second animation evaluator.
"""
import bpy, json, os, sys, math, importlib.util
sys.dont_write_bytecode=True
from mathutils import Matrix, Vector

root = os.path.abspath(sys.argv[sys.argv.index('--')+1] if '--' in sys.argv else '.')
spec = importlib.util.spec_from_file_location('building_geometry', os.path.join(root,'gbScripts/export-buildings.py'))
helper = importlib.util.module_from_spec(spec);spec.loader.exec_module(helper)
model = json.load(open(os.path.join(root,'gbResources/hand_cursor.bbmodel'),encoding='utf-8'))
bake = json.load(open(os.path.join(root,'gbResources/hand_cursor.animation.json')))
out = os.path.join(root,'gbDemo/output/hand-cursor');os.makedirs(out,exist_ok=True)
groups = {g['uuid']:g for g in model['groups']};owners={};parents={};transforms={}
def walk(nodes,parent=None,transform=Matrix.Identity(4)):
    for n in nodes:
        if isinstance(n,str):owners[n]=parent;transforms[n]=transform
        else:
            g=groups[n['uuid']];parents[g['name']]=parent
            walk(n.get('children',[]),g['name'],transform@helper.pivot_transform(g))
walk(model['outliner'])
scale=2.0/30.4
anchor=Vector((-3.2,24.8,-1))
C=Matrix(((-1,0,0,0),(0,0,1,0),(0,1,0,0),(0,0,0,1)))
convert=Matrix.Diagonal((scale,scale,scale,1))@C
offset=Matrix.Translation(-(convert@anchor))
def point(v):return offset@convert@Vector(v)
def native_matrix(values):return Matrix([values[i:i+4] for i in range(0,16,4)]).transposed()
def bone_matrix(values):return offset@convert@native_matrix(values)@convert.inverted()
bpy.ops.object.select_all(action='SELECT');bpy.ops.object.delete(use_global=False)
scene=bpy.context.scene;scene.render.fps=30;scene.frame_start=0;scene.frame_end=84
verts=[];faces=[];uvs=[];skin=[]
normals={'north':(0,0,-1),'south':(0,0,1),'east':(1,0,0),'west':(-1,0,0),'up':(0,1,0),'down':(0,-1,0)}
for e in model['elements']:
    a,b=e['from'],e['to'];transform=transforms[e['uuid']]@helper.pivot_transform(e);lookup={}
    for x in (a[0],b[0]):
        for y in (a[1],b[1]):
            for z in (a[2],b[2]):
                lookup[tuple(round(c,6) for c in (x,y,z))]=len(verts);verts.append(point(transform@Vector((x,y,z))));skin.append(owners[e['uuid']])
    for side,f in e['faces'].items():
        indices=[];coords=[]
        for u,v in ((0,0),(1,0),(1,1),(0,1)):
            indices.append(lookup[tuple(round(c,6) for c in helper.uv_corner(a,b,side,u,v,int(f.get('rotation',0))))]);r=f['uv']
            coords.append(((r[0]+(r[2]-r[0])*u)/32,1-(r[1]+(r[3]-r[1])*v)/32))
        normal=(verts[indices[1]]-verts[indices[0]]).cross(verts[indices[2]]-verts[indices[0]])
        if normal.dot(C.to_3x3()@transform.to_3x3()@Vector(normals[side]))<0:indices.reverse();coords.reverse()
        faces.append(indices);uvs.append(coords)
data=bpy.data.meshes.new('HandCursorGeometry');data.from_pydata(verts,[],faces);data.update()
mesh=bpy.data.objects.new('hand_cursor',data);scene.collection.objects.link(mesh)
layer=data.uv_layers.new(name='BlockbenchUV')
for face,uv in zip(data.polygons,uvs):
    for i,coord in zip(face.loop_indices,uv):layer.data[i].uv=coord
    face.use_smooth=False
mat=bpy.data.materials.new('HandCursorPixels');mat.use_nodes=True
texture=mat.node_tree.nodes.new('ShaderNodeTexImage');texture.image=bpy.data.images.load(os.path.join(root,'gbBundle/Textures/hand_cursor.png'));texture.interpolation='Closest'
mat.node_tree.links.new(texture.outputs['Color'],mat.node_tree.nodes.get('Principled BSDF').inputs['Base Color']);mesh.data.materials.append(mat)
arm=bpy.data.armatures.new('HandCursorSkeleton');rig=bpy.data.objects.new('HandCursorRig',arm);scene.collection.objects.link(rig)
bpy.context.view_layer.objects.active=rig;rig.select_set(True);bpy.ops.object.mode_set(mode='EDIT')
for r in bake['rest']:
    b=arm.edit_bones.new(r['name']);b.head=(0,0,0);b.tail=(0,.1,0);b.matrix=bone_matrix(r['matrix'])
    if parents[r['name']]:b.parent=arm.edit_bones[parents[r['name']]]
bpy.ops.object.mode_set(mode='OBJECT')
for r in bake['rest']:
    expected_rest=bone_matrix(r['matrix']);actual_rest=arm.bones[r['name']].matrix_local
    assert max(abs(actual_rest[a][b]-expected_rest[a][b]) for a in range(4) for b in range(4))<.00003,(r['name'],actual_rest,expected_rest)
for g in model['groups']:mesh.vertex_groups.new(name=g['name'])
for i,name in enumerate(skin):mesh.vertex_groups[name].add([i],1,'REPLACE')
mod=mesh.modifiers.new('HandSkin','ARMATURE');mod.object=rig;mesh.parent=rig
bpy.ops.object.select_all(action='DESELECT');mesh.select_set(True);rig.select_set(True)
fbx_dir=os.path.join(root,'gbBundle/Meshes')
export_args=dict(use_selection=True,object_types={'ARMATURE','MESH'},axis_forward='-Z',axis_up='Y',add_leaf_bones=False,apply_scale_options='FBX_SCALE_ALL',use_mesh_modifiers=False,path_mode='STRIP')
if '--animation-only' not in sys.argv:
    bpy.ops.export_scene.fbx(filepath=os.path.join(fbx_dir,'hand_cursor.fbx'),bake_anim=False,**export_args)
for frame in bake['frames']:
    scene.frame_set(round(frame['time']*30))
    targets={r['name']:bone_matrix(r['matrix']) for r in frame['bones']}
    for r in frame['bones']:
        b=rig.pose.bones[r['name']];b.rotation_mode='QUATERNION'
        b.matrix_basis=b.bone.matrix_local.inverted()@(b.parent.bone.matrix_local@targets[b.parent.name].inverted() if b.parent else Matrix.Identity(4))@targets[b.name]
        for channel in ('location','rotation_quaternion','scale'):b.keyframe_insert(channel,frame=scene.frame_current,group=b.name)
rig.animation_data.action.name='idle'
bpy.ops.export_scene.fbx(filepath=os.path.join(fbx_dir,'hand_cursor_idle.fbx'),bake_anim=True,bake_anim_use_all_actions=False,bake_anim_use_nla_strips=False,bake_anim_simplify_factor=0,**export_args)
# Compare every exported pose's deformation against the native Blockbench matrices.
rest={r['name']:bone_matrix(r['matrix']) for r in bake['rest']};worst=0
for frame in bake['frames']:
    scene.frame_set(round(frame['time']*30));bpy.context.view_layer.update()
    expected={r['name']:bone_matrix(r['matrix'])@rest[r['name']].inverted() for r in frame['bones']}
    evaluated=mesh.evaluated_get(bpy.context.evaluated_depsgraph_get()).to_mesh()
    for i,v in enumerate(verts):
        err=(evaluated.vertices[i].co-expected[skin[i]]@v).length
        if err>worst+.01:
            print('HAND_POSE_DIAGNOSTIC',scene.frame_current,skin[i],err,'vertex',tuple(v),tuple(evaluated.vertices[i].co),'expected',tuple(expected[skin[i]]@v),'matrix_error',max(abs(rig.pose.bones[skin[i]].matrix[a][b]-next(bone_matrix(r['matrix']) for r in frame['bones'] if r['name']==skin[i])[a][b]) for a in range(4) for b in range(4)),flush=True)
        worst=max(worst,err)
    mesh.evaluated_get(bpy.context.evaluated_depsgraph_get()).to_mesh_clear()
assert worst<.00003, worst
scene.frame_set(0)
world=bpy.data.worlds.new('HandPreviewWorld');scene.world=world;world.use_nodes=True
world.node_tree.nodes['Background'].inputs[0].default_value=(.3,.32,.35,1)
scene.render.engine='BLENDER_EEVEE';scene.render.resolution_x=scene.render.resolution_y=512;scene.render.resolution_percentage=100
scene.render.film_transparent=True;scene.view_settings.view_transform='Standard'
target=Vector([(min(v[i] for v in verts)+max(v[i] for v in verts))*.5 for i in range(3)])
camera_data=bpy.data.cameras.new('HandPreview');camera=bpy.data.objects.new('HandPreview',camera_data);scene.collection.objects.link(camera);scene.camera=camera
camera.location=target+Vector((2,6,1.7));camera.rotation_euler=(target-camera.location).to_track_quat('-Z','Y').to_euler()
camera_data.type='ORTHO';camera_data.ortho_scale=2.5
for name,pos,energy in [('Key',(-3,-4,5),500),('Fill',(3,-2,2),250)]:
    light_data=bpy.data.lights.new(name,'AREA');light_data.energy=energy;light_data.size=4
    light=bpy.data.objects.new(name,light_data);scene.collection.objects.link(light);light.location=target+Vector(pos);light.rotation_euler=(target-light.location).to_track_quat('-Z','Y').to_euler()
scene.render.filepath=os.path.join(out,'preview.png');bpy.ops.render.render(write_still=True)
json.dump({'cubes':len(model['elements']),'triangles':len(faces)*2,'bones':len(groups),'frames':85,'native_pose_error_m':worst,'tip_origin':[0,0,0],'length_m':2},open(os.path.join(out,'export.json'),'w'),indent=2)
print('HAND_EXPORT PASS native animation error',worst,flush=True)
