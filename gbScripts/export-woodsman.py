"""Blender: preserve Blockbench cube geometry/UVs, add a Game Box humanoid rig.
--background --python-exit-code 1 --python gbScripts/export-woodsman.py -- --root .
Cube UV interpretation follows Blockbench js/outliner/types/cube.js UVToLocal.
"""
import argparse, base64, hashlib, json, math, os, shutil, sys
import bpy
from mathutils import Vector

p=argparse.ArgumentParser();p.add_argument('--root',default='.')
args=p.parse_args(sys.argv[sys.argv.index('--')+1:] if '--' in sys.argv else [])
root=os.path.abspath(args.root)
source=os.path.join(root,'gbResources','woodsman.bbmodel')
with open(source,encoding='utf-8-sig') as f: model=json.load(f)
geometry_before=json.dumps({k:v for k,v in model.items() if k!='animations'},sort_keys=True)
out=os.path.join(root,'gbResources','woodsman');os.makedirs(out,exist_ok=True)
mesh_dir=os.path.join(root,'gbBundle','Meshes');tex_dir=os.path.join(root,'gbBundle','Textures')
preview=os.path.join(root,'gbDemo','output','island-check','woodsman');os.makedirs(preview,exist_ok=True)
assert len(model['elements'])==130 and all(e['type']=='cube' for e in model['elements'])
assert all(not any(g.get('rotation',[0,0,0])) for g in model['groups'])
assert all(not any(e.get('rotation',[0,0,0])) and not e.get('inflate',0) for e in model['elements'])
assert len(model['textures'])==1
scale=1.9/47.0
# Blockbench front -Z -> Game Box front +Z, both Y up. Blender uses Z up:
# BB (x,y,z) -> Blender (-x,z,y) -> imported Game Box (-x,y,-z).
def point(v): return Vector((-v[0]*scale,v[2]*scale,v[1]*scale))
groups={g['uuid']:g for g in model['groups']}; owners={}
def walk(nodes,group=None):
    for n in nodes:
        if isinstance(n,str): owners[n]=group
        else: walk(n.get('children',[]),groups[n['uuid']]['name'])
walk(model['outliner']);assert all(e['uuid'] in owners for e in model['elements'])
# This model's BB side labels are opposite to anatomical/Mixamo sides.
# After the proper yaw conversion, a +Z-facing character's left is +X.
mapping={'head':'head','right_arm':'upper_arm_l','right_forearm':'lower_arm_l','right_hand':'hand_l',
         'left_arm':'upper_arm_r','left_forearm':'lower_arm_r','left_hand':'hand_r',
         'right_leg':'upper_leg_l','right_shin':'lower_leg_l','right_foot':'foot_l',
         'left_leg':'upper_leg_r','left_shin':'lower_leg_r','left_foot':'foot_r'}
def weights(e,v):
    if e['name']=='neck': return {'neck':1.0}
    owner=owners[e['uuid']]
    y=v[1]
    # Adjacent cubes share one field through each joint; rigid weights away
    # from the joint keep the authored cuboid silhouette intact.
    for side in ('left','right'):
        suffix='_r' if side=='left' else '_l'
        if owner in (side+'_arm',side+'_forearm',side+'_hand'):
            upper,lower,hand=('upper_arm'+suffix,'lower_arm'+suffix,'hand'+suffix)
            if y>=26:return {upper:1.0}
            if y>24:t=(26-y)/2.;return {upper:1-t,lower:t}
            if y>=19:return {lower:1.0}
            if y>16:t=(19-y)/3.;return {lower:1-t,hand:t}
            return {hand:1.0}
        if owner in (side+'_leg',side+'_shin',side+'_foot'):
            upper,lower,foot=('upper_leg'+suffix,'lower_leg'+suffix,'foot'+suffix)
            if y>=12:return {upper:1.0}
            if y>9:t=(12-y)/3.;return {upper:1-t,lower:t}
            if y>=6:return {lower:1.0}
            if y>4:t=(6-y)/2.;return {lower:1-t,foot:t}
            return {foot:1.0}
    if owner!='body': return {mapping[owner]:1.0}
    # All torso accessories follow the same vertical field, preserving seams.
    if y<=20:return {'hips':1.0}
    if y<25: t=(y-20)/5.;return {'hips':1-t,'spine':t}
    if y<29: t=(y-25)/4.;return {'spine':1-t,'chest':t}
    return {'chest':1.0}

bpy.ops.object.select_all(action='SELECT');bpy.ops.object.delete(use_global=False)
bpy.context.preferences.filepaths.save_version=0
scene=bpy.context.scene;scene.unit_settings.system='METRIC';scene.unit_settings.scale_length=1
verts=[];faces=[];uvs=[];skin=[];expected=[]
normals={'north':(0,0,-1),'south':(0,0,1),'east':(1,0,0),'west':(-1,0,0),'up':(0,1,0),'down':(0,-1,0)}
for e in model['elements']:
    assert e.get('export',True) and e.get('visibility',True)
    a,b=e['from'],e['to'];lookup={}
    for x in (a[0],b[0]):
        for y in (a[1],b[1]):
            for z in (a[2],b[2]):
                v=(x,y,z);lookup[v]=len(verts);verts.append(point(v));skin.append(weights(e,v))
                expected.append((-x*scale,y*scale,-z*scale))
    for side,f in e['faces'].items():
        if f.get('texture') is None or not f.get('enabled',True):continue
        assert f.get('texture')==0
        corners=[];face_uv=[]
        for u,v in ((0,0),(1,0),(1,1),(0,1)):
            ru,rv=u,v
            for _ in range(int(f.get('rotation',0))//90):ru,rv=1-rv,ru
            if side=='north': pos=(b[0]+(a[0]-b[0])*ru,b[1]+(a[1]-b[1])*rv,a[2])
            elif side=='south':pos=(a[0]+(b[0]-a[0])*ru,b[1]+(a[1]-b[1])*rv,b[2])
            elif side=='east':pos=(b[0],b[1]+(a[1]-b[1])*rv,b[2]+(a[2]-b[2])*ru)
            elif side=='west':pos=(a[0],b[1]+(a[1]-b[1])*rv,a[2]+(b[2]-a[2])*ru)
            elif side=='up':pos=(a[0]+(b[0]-a[0])*ru,b[1],a[2]+(b[2]-a[2])*rv)
            elif side=='down':pos=(a[0]+(b[0]-a[0])*ru,a[1],b[2]+(a[2]-b[2])*rv)
            else:raise ValueError(side)
            corners.append(lookup[pos]);rect=f['uv'];face_uv.append(((rect[0]+(rect[2]-rect[0])*u)/128.,1-(rect[1]+(rect[3]-rect[1])*v)/128.))
        n=(verts[corners[1]]-verts[corners[0]]).cross(verts[corners[2]]-verts[corners[0]])
        if n.dot(point(normals[side]))<0:corners.reverse();face_uv.reverse()
        faces.append(corners);uvs.append(face_uv)
data=bpy.data.meshes.new('WoodsmanFinalGeometry');data.from_pydata(verts,[],faces);data.update()
mesh=bpy.data.objects.new('woodsman',data);scene.collection.objects.link(mesh)
layer=data.uv_layers.new(name='BlockbenchUV')
for face,uv in zip(data.polygons,uvs):
    for i,coord in zip(face.loop_indices,uv):layer.data[i].uv=coord
    face.use_smooth=False
data.calc_loop_triangles();assert len(data.loop_triangles)==1560 and len(data.vertices)==1040
assert all((v.co-verts[i]).length<1e-7 for i,v in enumerate(data.vertices))
png=base64.b64decode(model['textures'][0]['source'].split(',',1)[1])
texture_path=os.path.join(tex_dir,'woodsman.png')
with open(texture_path,'wb') as f:f.write(png)
image=bpy.data.images.load(texture_path);image.pack()
mat=bpy.data.materials.new('WoodsmanOriginalTexture');mat.use_nodes=True
tex=mat.node_tree.nodes.new('ShaderNodeTexImage');tex.image=image;tex.interpolation='Closest'
mat.node_tree.links.new(tex.outputs['Color'],mat.node_tree.nodes.get('Principled BSDF').inputs['Base Color'])
mesh.data.materials.append(mat)
mask=bpy.data.images.new('woodsman_no_recolor',width=1,height=1,alpha=True)
mask.pixels=[0,0,0,1];mask.filepath_raw=os.path.join(tex_dir,'woodsman_no_recolor.png');mask.file_format='PNG';mask.save()
arm=bpy.data.armatures.new('WoodsmanSkeleton');rig=bpy.data.objects.new('WoodsmanRig',arm);scene.collection.objects.link(rig)
bpy.context.view_layer.objects.active=rig;rig.select_set(True);bpy.ops.object.mode_set(mode='EDIT')
bones={}
def bone(name,h,t,parent=None):
    b=arm.edit_bones.new(name);b.head=point(h);b.tail=point(t)
    if parent:b.parent=bones[parent]
    bones[name]=b
bone('root',(0,0,0),(0,3,0))
bone('hips',(0,20,0),(0,25,0),'root')
bone('spine',(0,25,0),(0,29,0),'hips')
bone('chest',(0,29,0),(0,32,0),'spine')
bone('neck',(0,32,0),(0,34,0),'chest')
bone('head',(0,34,0),(0,45,0),'neck')
for side,sign in (('l',-1),('r',1)):
    bone('upper_arm_'+side,(sign*8,32,0),(sign*10,25,0),'chest')
    bone('lower_arm_'+side,(sign*10,25,0),(sign*11,18,0),'upper_arm_'+side)
    bone('hand_'+side,(sign*11,18,0),(sign*11,12,0),'lower_arm_'+side)
    bone('upper_leg_'+side,(sign*3,20,0),(sign*3,11,0),'hips')
    bone('lower_leg_'+side,(sign*3,11,0),(sign*3,3,0),'upper_leg_'+side)
    bone('foot_'+side,(sign*3,3,0),(sign*3,1,-6),'lower_leg_'+side)
bpy.ops.object.mode_set(mode='OBJECT')
for name in bones:mesh.vertex_groups.new(name=name)
for i,w in enumerate(skin):
    assert abs(sum(w.values())-1)<1e-7
    for name,value in w.items():
        if value>0:mesh.vertex_groups[name].add([i],value,'REPLACE')
mod=mesh.modifiers.new('WoodsmanSkin','ARMATURE');mod.object=rig;mesh.parent=rig
assert not bpy.data.actions and not rig.animation_data
bpy.ops.object.select_all(action='DESELECT');mesh.select_set(True);rig.select_set(True)
fbx=os.path.join(mesh_dir,'woodsman.fbx')
bpy.ops.export_scene.fbx(filepath=fbx,use_selection=True,object_types={'ARMATURE','MESH'},axis_forward='-Z',axis_up='Y',
    add_leaf_bones=False,apply_scale_options='FBX_SCALE_ALL',bake_anim=False,use_mesh_modifiers=False,path_mode='STRIP')
shutil.copyfile(fbx,os.path.join(root,'gbResources','woodsman.fbx'))
bpy.ops.wm.save_as_mainfile(filepath=os.path.join(out,'woodsman.blend'))
with open(os.path.join(out,'expected_vertices.txt'),'w') as f:
    for v in expected:f.write(' '.join(format(c,'.10f') for c in v)+'\n')
with open(os.path.join(out,'expected_uv_corners.txt'),'w') as f:
    for face,coords in zip(faces,uvs):
        for i,uv in zip(face,coords):
            f.write(' '.join(format(c,'.10f') for c in expected[i]+uv)+'\n')
report={'cubes':130,'vertices':len(verts),'faces':len(faces),'triangles':len(data.loop_triangles),
        'height_m':1.9,'blockbench_unit_m':scale,'bones':list(bones),'animations':[],
        'geometry_sha256':hashlib.sha256(geometry_before.encode()).hexdigest(),'texture_sha256':hashlib.sha256(png).hexdigest()}
with open(os.path.join(out,'export.json'),'w') as f:json.dump(report,f,indent=2)
# Delete only the embedded animation; preserve every other model field verbatim semantically.
model['animations']=[]
assert json.dumps({k:v for k,v in model.items() if k!='animations'},sort_keys=True)==geometry_before
with open(source,'w',encoding='utf-8') as f:json.dump(model,f,ensure_ascii=False,indent=2);f.write('\n')
print('WOODSMAN_EXPORT '+json.dumps(report))
# Neutral source pose preview, separate from the saved export scene.
world=scene.world or bpy.data.worlds.new('World');scene.world=world;world.use_nodes=True
world.node_tree.nodes['Background'].inputs[0].default_value=(.2,.25,.3,1)
scene.render.engine='BLENDER_EEVEE';scene.render.resolution_x=640;scene.render.resolution_y=800;scene.render.resolution_percentage=100
scene.view_settings.view_transform='Standard'
light_data=bpy.data.lights.new('Key','AREA');light_data.energy=450;light_data.size=5
light=bpy.data.objects.new('Key',light_data);scene.collection.objects.link(light);light.location=(2,-3,4)
cam_data=bpy.data.cameras.new('Preview');cam=bpy.data.objects.new('Preview',cam_data);scene.collection.objects.link(cam);scene.camera=cam
cam_data.type='ORTHO';cam_data.ortho_scale=2.3
for name,location in [('front',(0,-5,1)),('side',(5,0,1)),('back',(0,5,1))]:
    cam.location=location;cam.rotation_euler=(Vector((0,0,.95))-cam.location).to_track_quat('-Z','Y').to_euler()
    scene.render.filepath=os.path.join(preview,name+'.png');bpy.ops.render.render(write_still=True)
