"""Original low-poly male resident, rig, in-place animations and FBX exports.
Blender --background --python-exit-code 1 --python this-file -- --output-root .
"""
import bpy, math, os, sys, argparse
bpy.context.preferences.filepaths.save_version=0
from mathutils import Vector
args=argparse.ArgumentParser()
args.add_argument('--output-root',default='.')
args.add_argument('--preview-dir',default='gbDemo/output/island-check/resident-asset')
opts=args.parse_args(sys.argv[sys.argv.index('--')+1:] if '--' in sys.argv else [])
root=os.path.abspath(opts.output_root)
mesh_dir=os.path.join(root,'gbBundle','Meshes')
texture_dir=os.path.join(root,'gbBundle','Textures')
source_dir=os.path.join(root,'gbResources','island_resident')
preview=os.path.join(root,opts.preview_dir)
for path in [mesh_dir,texture_dir,source_dir,preview]: os.makedirs(path,exist_ok=True)
bpy.ops.object.select_all(action='SELECT');bpy.ops.object.delete(use_global=False)
scene=bpy.context.scene
scene.unit_settings.system='METRIC';scene.unit_settings.scale_length=1
scene.render.fps=30
verts=[];faces=[];weights=[];patches=[];regions=[]
# UV palette patches: clothing white, skin, boot leather, hair, eyes, pupils, belt, soles.
colors=[(1,1,1,1),(.64,.43,.24,1),(.17,.08,.035,1),(.13,.065,.025,1),
        (.92,.92,.8,1),(.012,.018,.012,1),(.27,.13,.045,1),(.08,.045,.025,1)]
def vertex(p,bone):
    verts.append(tuple(p));weights.append(bone);return len(verts)-1
def face(indices,patch,region=''):
    for i in range(1,len(indices)-1):
        faces.append((indices[0],indices[i],indices[i+1]));patches.append(patch);regions.append(region)
def loft(points,radii,bone,patch,region='',n=12):
    rings=[]
    for p,r in zip(points,radii):
        rings.append([vertex((p[0]+math.cos(j*math.tau/n)*r[0],
                              p[1]+math.sin(j*math.tau/n)*r[1],p[2]),bone) for j in range(n)])
    face(list(reversed(rings[0])),patch,region)
    for a,b in zip(rings,rings[1:]):
        for j in range(n): face([a[j],a[(j+1)%n],b[(j+1)%n],b[j]],patch,region)
    face(rings[-1],patch,region)
def limb(a,b,radius,bone,patch,region=''):
    # Horizontal cross sections overlap the adjacent joint to avoid exposed seams.
    pts=[Vector(a).lerp(Vector(b),i/3) for i in range(4)]
    loft(pts,[(radius*.95,radius*.85),(radius,radius*.9),
              (radius*.9,radius*.85),(radius*.82,radius*.8)],bone,patch,region)
loft([(0,0,1.055),(0,0,1.10),(0,0,1.18)],[(.13,.085),(.145,.09),(.125,.075)],'pelvis',0,'trousers')
loft([(0,0,1.13),(0,0,1.25),(0,0,1.43),(0,0,1.51)],
     [(.132,.08),(.145,.087),(.20,.105),(.18,.095)],'spine',0,'shirt')
loft([(0,0,1.49),(0,0,1.59),(0,0,1.65)],[(.055,.05),(.047,.045),(.055,.05)],'neck',1)
# Angular jaw, cheekbones and forehead.
loft([(0,-.01,1.61),(0,-.016,1.66),(0,0,1.77),(0,.005,1.84),(0,.005,1.86)],
     [(.047,.04),(.073,.067),(.09,.08),(.076,.069),(.035,.037)],'head',1,n=12)
loft([(0,.006,1.79),(0,.012,1.84),(0,.018,1.88),(0,.025,1.90)],
     [(.088,.079),(.08,.072),(.055,.06),(.009,.009)],'head',3,n=12)
# Face components are geometric so palette textures remain simple.
for x in [-.038,.038]:
    loft([(x,-.077,1.752),(x,-.084,1.775)],[(.019,.006),(.017,.007)],'head',4,n=8)
    loft([(x,-.087,1.756),(x,-.091,1.772)],[(.008,.004),(.007,.004)],'head',5,n=8)
    loft([(x,-.076,1.783),(x,-.078,1.793)],[(.023,.009),(.021,.009)],'head',3,n=8)
loft([(0,-.077,1.701),(0,-.100,1.727),(0,-.079,1.749)],
     [(.016,.01),(.018,.022),(.014,.009)],'head',1,n=6)
loft([(0,-.078,1.677),(0,-.079,1.683)],[(.022,.006),(.020,.006)],'head',6,n=8)
loft([(0,0,1.12),(0,0,1.142)],[(.152,.098),(.151,.097)],'pelvis',6)
for side,sign in [('L',1),('R',-1)]:
    shoulder=(sign*.195,0,1.46);elbow=(sign*.26,0,1.18);wrist=(sign*.28,-.006,.945)
    limb(shoulder,(sign*.235,0,1.31),.065,'upper_arm.'+side,0,'shirt')
    limb((sign*.23,0,1.35),elbow,.046,'upper_arm.'+side,1)
    limb(elbow,wrist,.037,'forearm.'+side,1)
    loft([(wrist[0],-.005,.96),(sign*.282,-.008,.895),(sign*.277,-.007,.862)],
         [(.03,.023),(.035,.024),(.022,.018)],'hand.'+side,1,n=10)
    hip=(sign*.085,0,1.10);knee=(sign*.105,0,.72);ankle=(sign*.11,0,.22)
    limb(hip,knee,.076,'thigh.'+side,0,'trousers')
    limb(knee,(sign*.11,0,.27),.052,'shin.'+side,0,'trousers')
    loft([(sign*.11,-.038,.04),(sign*.11,-.045,.085),(sign*.11,-.02,.18),(sign*.11,0,.33)],
         [(.065,.115),(.064,.125),(.046,.064),(.057,.059)],'foot.'+side,2,n=12)
    loft([(sign*.11,-.048,0),(sign*.11,-.048,.045)],[(.066,.116),(.067,.117)],'foot.'+side,7)
data=bpy.data.meshes.new('ResidentMesh');data.from_pydata(verts,[],faces);data.update()
mesh=bpy.data.objects.new('IslandResident',data);scene.collection.objects.link(mesh)
uv=data.uv_layers.new(name='UVMap')
for polygon,patch in zip(data.polygons,patches):
    for loop in polygon.loop_indices: uv.data[loop].uv=((patch+.5)/8,.5)
for bone in sorted(set(weights)):
    group=mesh.vertex_groups.new(name=bone)
    group.add([i for i,b in enumerate(weights) if b==bone],1,'REPLACE')
mesh['shirt_faces']=regions.count('shirt');mesh['trousers_faces']=regions.count('trousers')
def texture(name,mask=False):
    image=bpy.data.images.new(name,width=256,height=32,alpha=True)
    image.colorspace_settings.name='Non-Color'
    values=[]
    for y in range(32):
        for x in range(256): values.extend(((1,1,1,1) if x//32==0 else (0,0,0,1)) if mask else colors[x//32])
    image.pixels.foreach_set(values);image.filepath_raw=os.path.join(texture_dir,name+'.png');image.file_format='PNG';image.save()
    return image
palette=texture('island_resident_palette');mask=texture('island_resident_clothing_mask',True)
material=bpy.data.materials.new('ResidentPalette');material.use_nodes=True
nodes=material.node_tree.nodes;links=material.node_tree.links
base=nodes.new('ShaderNodeTexImage');base.image=palette;base.interpolation='Closest'
clothing=nodes.new('ShaderNodeTexImage');clothing.image=mask;clothing.interpolation='Closest'
mix=nodes.new('ShaderNodeMixRGB');mix.blend_type='MIX';mix.inputs[2].default_value=(.055,.23,.43,1)
links.new(clothing.outputs['Color'],mix.inputs[0]);links.new(base.outputs['Color'],mix.inputs[1])
links.new(mix.outputs[0],nodes.get('Principled BSDF').inputs['Base Color'])
nodes.get('Principled BSDF').inputs['Roughness'].default_value=.85
mesh.data.materials.append(material)
arm=bpy.data.armatures.new('ResidentArmature');rig=bpy.data.objects.new('ResidentRig',arm);scene.collection.objects.link(rig)
bpy.context.view_layer.objects.active=rig;rig.select_set(True);bpy.ops.object.mode_set(mode='EDIT')
bones={}
def bone(name,head,tail,parent=None):
    b=arm.edit_bones.new(name);b.head=head;b.tail=tail
    if parent:b.parent=bones[parent]
    bones[name]=b
bone('root',(0,0,0),(0,0,.15))
bone('pelvis',(0,0,1.055),(0,0,1.15),'root')
bone('spine',(0,0,1.15),(0,0,1.49),'pelvis')
bone('neck',(0,0,1.49),(0,0,1.63),'spine')
bone('head',(0,0,1.63),(0,0,1.86),'neck')
for side,sign in [('L',1),('R',-1)]:
    bone('upper_arm.'+side,(sign*.195,0,1.46),(sign*.26,0,1.18),'spine')
    bone('forearm.'+side,(sign*.26,0,1.18),(sign*.28,-.006,.945),'upper_arm.'+side)
    bone('hand.'+side,(sign*.28,-.006,.945),(sign*.28,-.006,.87),'forearm.'+side)
    bone('thigh.'+side,(sign*.085,0,1.10),(sign*.105,0,.72),'pelvis')
    bone('shin.'+side,(sign*.105,0,.72),(sign*.11,0,.22),'thigh.'+side)
    bone('foot.'+side,(sign*.11,0,.22),(sign*.11,-.13,.07),'shin.'+side)
bpy.ops.object.mode_set(mode='OBJECT')
modifier=mesh.modifiers.new('ResidentSkin','ARMATURE');modifier.object=rig;mesh.parent=rig
rig.animation_data_create()
for clip,duration in [('idle',90),('walk',24)]:
    action=bpy.data.actions.new(clip);rig.animation_data.action=action
    for frame in range(duration+1):
        phase=math.tau*frame/duration
        for pb in rig.pose.bones:
            pb.rotation_mode='XYZ';pb.rotation_euler=(0,0,0);pb.location=(0,0,0)
        if clip=='idle':
            rig.pose.bones['spine'].rotation_euler.x=.018*math.sin(phase)
            rig.pose.bones['head'].rotation_euler.z=.035*math.sin(phase)
            rig.pose.bones['pelvis'].location.y=.006*math.sin(phase)
            for side,sign in [('L',1),('R',-1)]:
                rig.pose.bones['upper_arm.'+side].rotation_euler.y=.04*sign
        else:
            for side,sign in [('L',1),('R',-1)]:
                swing=math.sin(phase)*sign
                rig.pose.bones['thigh.'+side].rotation_euler.x=.40*swing
                rig.pose.bones['shin.'+side].rotation_euler.x=-.65*max(-swing,0)
                rig.pose.bones['foot.'+side].rotation_euler.x=.18*max(-swing,0)
                rig.pose.bones['upper_arm.'+side].rotation_euler.x=-.28*swing
                rig.pose.bones['forearm.'+side].rotation_euler.x=-.1-.12*max(swing,0)
            rig.pose.bones['pelvis'].location.y=.008*(1-math.cos(phase*2))
            rig.pose.bones['spine'].rotation_euler.z=.025*math.sin(phase)
        for pb in rig.pose.bones:
            pb.keyframe_insert('rotation_euler',frame=frame,group=pb.name)
            pb.keyframe_insert('location',frame=frame,group=pb.name)
    action.use_fake_user=True
def export(name,clip=None):
    rig.animation_data.action=bpy.data.actions.get(clip) if clip else None
    scene.frame_start=0;scene.frame_end=90 if clip=='idle' else 24
    scene.frame_set(0)
    if not clip:
        for pb in rig.pose.bones:pb.rotation_euler=(0,0,0);pb.location=(0,0,0)
    bpy.ops.object.select_all(action='DESELECT');rig.select_set(True);mesh.select_set(True);bpy.context.view_layer.objects.active=rig
    bpy.ops.export_scene.fbx(filepath=os.path.join(mesh_dir,name+'.fbx'),use_selection=True,
        object_types={'ARMATURE','MESH'},axis_forward='-Z',axis_up='Y',add_leaf_bones=False,
        apply_scale_options='FBX_SCALE_ALL',
        bake_anim=clip is not None,bake_anim_use_all_actions=False,bake_anim_use_nla_strips=False,
        bake_anim_simplify_factor=0,use_mesh_modifiers=False,path_mode='STRIP')
export('island_resident');export('island_resident_idle','idle');export('island_resident_walk','walk')
rig.animation_data.action=bpy.data.actions['idle'];scene.frame_set(0)
# Authoring previews use the original geometry and palette material.
scene.render.engine='CYCLES';scene.cycles.samples=24
scene.render.resolution_x=480;scene.render.resolution_y=640;scene.render.resolution_percentage=100
scene.world.color=(.35,.35,.35)
for location,energy,size in [((2,-3,4),450,4),((-3,-1,2.5),180,3)]:
    light=bpy.data.lights.new('Softbox','AREA');light.energy=energy;light.shape='DISK';light.size=size
    obj=bpy.data.objects.new('Softbox',light);scene.collection.objects.link(obj);obj.location=location
    obj.rotation_euler=(Vector((0,0,1))-obj.location).to_track_quat('-Z','Y').to_euler()
camdata=bpy.data.cameras.new('PreviewCamera');cam=bpy.data.objects.new('PreviewCamera',camdata);scene.collection.objects.link(cam)
camdata.type='ORTHO';camdata.ortho_scale=2.2;scene.camera=cam
cam.location=(0,-4,1.05);cam.rotation_euler=(Vector((0,0,1))-cam.location).to_track_quat('-Z','Y').to_euler()
for image in bpy.data.images:
    if image.source=='FILE':image.pack()
bpy.ops.wm.save_as_mainfile(filepath=os.path.join(source_dir,'island_resident.blend'))
for name,location in [('front',(0,-4,1.05)),('side',(4,0,1.05)),('back',(0,4,1.05))]:
    cam.location=location;cam.rotation_euler=(Vector((0,0,1))-cam.location).to_track_quat('-Z','Y').to_euler()
    scene.render.filepath=os.path.join(preview,name+'.png');bpy.ops.render.render(write_still=True)
print('Resident generation complete: %d triangles, %d bones' % (len(faces),len(arm.bones)))
