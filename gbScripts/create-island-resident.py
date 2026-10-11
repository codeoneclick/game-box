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
# Palette: shared clothing color, skin, boots, hair, spare patches, belt, soles.
colors=[(1,1,1,1),(.88,.59,.31,1),(.22,.10,.035,1),(.43,.20,.055,1),
        (.92,.92,.8,1),(.012,.018,.012,1),(.27,.13,.045,1),(.08,.045,.025,1)]
def vertex(p,bone):
    verts.append(tuple(p));weights.append(bone(p) if callable(bone) else {bone:1.0});return len(verts)-1
def face(indices,patch,region=''):
    for i in range(1,len(indices)-1):
        faces.append((indices[0],indices[i],indices[i+1]));patches.append(patch);regions.append(region)
# Blueprint (2): 668 px total height, shoulder 281, waist 409,
# hem 535, knee 615 and boot top 665. All dimensions are authored at 1.9 m.
# Use sparse cross sections, not decimation, to preserve these landmarks.
def profile(n):
    if n==4:return [(-1,-1),(1,-1),(1,1),(-1,1)]
    if n==6:return [(-1,-.45),(-.65,-1),(.65,-1),(1,-.45),(.85,1),(-.85,1)]
    if n==8:return [(-1,-.55),(-.7,-1),(.7,-1),(1,-.55),(1,.55),(.7,1),(-.7,1),(-1,.55)]
    raise ValueError(n)

def connect(a,b,patch,region=''):
    for j in range(len(a)):face([a[j],a[(j+1)%len(a)],b[(j+1)%len(a)],b[j]],patch,region)

def loft(points,radii,bone,patch,region='',n=4,caps=True):
    rings=[]
    for p,r in zip(points,radii):
        rings.append([vertex((p[0]+x*r[0],p[1]+y*r[1],p[2]),bone) for x,y in profile(n)])
    for a,b in zip(rings,rings[1:]):connect(a,b,patch,region)
    if caps:face(list(reversed(rings[0])),patch,region);face(rings[-1],patch,region)
    return rings

def tube(a,b,radii,bone,patch,n=4):
    a,b=Vector(a),Vector(b);axis=(b-a).normalized()
    helper=Vector((0,0,1)) if abs(axis.z)<.9 else Vector((0,1,0))
    u=axis.cross(helper).normalized();v=axis.cross(u).normalized();rings=[]
    for i,r in enumerate(radii):
        center=a.lerp(b,i/(len(radii)-1))
        rings.append([vertex(center+u*x*r[0]+v*y*r[1],bone) for x,y in profile(n)])
    for a_ring,b_ring in zip(rings,rings[1:]):connect(a_ring,b_ring,patch)
    face(list(reversed(rings[0])),patch);face(rings[-1],patch)

def hem_weights(p):
    side='L' if p[0]>=0 else 'R'
    leg=max(0,min(.72,(1.04-p[2])/.34))*min(1,abs(p[0])/.06)
    return {'pelvis':1-leg,'thigh.'+side:leg}

# A real V-shaped opening; its skin triangle fills the opening, without coplanar overlays.
shirt=[]
for z,w,d in [(1.08,.128,.069),(1.30,.169,.077),(1.46,.202,.084)]:
    outline=[(-1,-1),(-.34,-1),(0,-1),(.34,-1),(1,-1),(1,1),(0,1),(-1,1)]
    shirt.append([vertex((x*w,y*d,1.365 if z==1.46 and x==0 and y==-1 else z),'spine') for x,y in outline])
for a,b in zip(shirt,shirt[1:]):connect(a,b,0,'shirt')
face([shirt[-1][1],shirt[-1][2],shirt[-1][3]],1)
face([shirt[-1][0],shirt[-1][1],shirt[-1][7]],0,'shirt')
face([shirt[-1][3],shirt[-1][4],shirt[-1][5]],0,'shirt')
face([shirt[-1][1],shirt[-1][3],shirt[-1][5],shirt[-1][7]],0,'shirt')
loft([(0,0,.744),(0,0,.90),(0,0,1.08)],[(.197,.087),(.161,.079),(.129,.069)],hem_weights,0,'trousers',n=6,caps=False)
loft([(0,0,1.025),(0,0,1.108)],[(.145,.087),(.139,.090)],'pelvis',6)
loft([(0,0,1.43),(0,0,1.615)],[(.047,.042),(.044,.040)],'neck',1,n=6)
loft([(0,-.012,1.55),(0,-.012,1.595),(0,-.014,1.70),(0,-.006,1.785)],
     [(.079,.056),(.123,.094),(.132,.097),(.132,.098)],'head',1,n=8)
# Hair cap dips to the nape and is raised over the forehead, as in the side view.
hair=[]
for layer in range(2):
    ring=[]
    for x,y in profile(8):
        front=max(0,-y)
        p=(x*(.137 if layer==0 else .125),.009+y*(.105 if layer==0 else .095),
           (1.625+front*.166) if layer==0 else (1.812+front*.014))
        ring.append(vertex(p,'head'))
    hair.append(ring)
connect(hair[0],hair[1],3)
peak=vertex((-.115,-.105,1.90),'head')
for j in range(8):face([hair[-1][j],hair[-1][(j+1)%8],peak],3)
for side,sign in [('L',1),('R',-1)]:
    # Small ears remain angular and do not add facial features.
    tube((sign*.126,-.002,1.65),(sign*.138,-.003,1.70),[(.009,.011),(.006,.009)],'head',1)
    shoulder=(sign*.190,0,1.46);elbow=(sign*.425,0,1.46);wrist=(sign*.640,0,1.46)
    def arm_weights(p,side=side):
        t=max(0,min(1,(abs(p[0])-.405)/.040))
        return {'upper_arm.'+side:1-t,'forearm.'+side:t}
    tube(shoulder,wrist,[(.043,.054),(.036,.046),(.030,.039)],arm_weights,1,n=6)
    tube((sign*.548,0,1.46),(sign*.655,0,1.46),[(.034,.052),(.035,.052)],'forearm.'+side,2)
    tube((sign*.646,-.008,1.46),(sign*.754,-.008,1.46),
         [(.023,.030),(.028,.036),(.007,.016)],'hand.'+side,1,n=6)
    def leg_weights(p,side=side):
        t=max(0,min(1,(p[2]-.485)/.070))
        return {'thigh.'+side:t,'shin.'+side:1-t}
    # Hide the upper cap inside the hem; skin above it would poke through the
    # narrow waist during deformation, although the thigh bone continues up.
    loft([(sign*.108,0,.35),(sign*.092,0,.52),(sign*.079,0,.66),(sign*.075,0,.80)],
         [(.043,.037),(.044,.038),(.050,.042),(.052,.044)],leg_weights,1)
    def boot_weights(p,side=side):
        shin=max(0,min(1,(p[2]-.08)/.095))
        return {'foot.'+side:1-shin,'shin.'+side:shin}
    loft([(sign*.135,-.045,0),(sign*.135,-.045,.040),(sign*.119,-.007,.130),(sign*.107,0,.375)],
         [(.108,.123),(.101,.123),(.061,.054),(.056,.051)],boot_weights,2,n=6)
assert 400<=len(faces)<=500, 'Authoring triangle budget: %d' % len(faces)

data=bpy.data.meshes.new('ResidentMesh');data.from_pydata(verts,[],faces);data.update()
mesh=bpy.data.objects.new('IslandResident',data);scene.collection.objects.link(mesh)
uv=data.uv_layers.new(name='UVMap')
for polygon,patch in zip(data.polygons,patches):
    for loop in polygon.loop_indices:uv.data[loop].uv=((patch+.5)/8,.5)
for bone in sorted({name for w in weights for name in w}):
    group=mesh.vertex_groups.new(name=bone)
    for i,w in enumerate(weights):
        if w.get(bone,0)>0:group.add([i],w[bone],'REPLACE')
mesh['shirt_faces']=regions.count('shirt');mesh['trousers_faces']=regions.count('trousers')
def texture(name,mask=False):
    image=bpy.data.images.new(name,width=256,height=32,alpha=True)
    image.colorspace_settings.name='Non-Color'
    values=[]
    for y in range(32):
        for x in range(256):values.extend(((1,1,1,1) if x//32==0 else (0,0,0,1)) if mask else colors[x//32])
    image.pixels.foreach_set(values);image.filepath_raw=os.path.join(texture_dir,name+'.png');image.file_format='PNG';image.save()
    return image
palette=texture('island_resident_palette');mask=texture('island_resident_clothing_mask',True)
material=bpy.data.materials.new('ResidentPalette');material.use_nodes=True
nodes=material.node_tree.nodes;links=material.node_tree.links
base=nodes.new('ShaderNodeTexImage');base.image=palette;base.interpolation='Closest'
clothing=nodes.new('ShaderNodeTexImage');clothing.image=mask;clothing.interpolation='Closest'
mix=nodes.new('ShaderNodeMixRGB');mix.blend_type='MIX';mix.inputs[2].default_value=(.035,.12,.30,1)
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
bone('pelvis',(0,0,1.025),(0,0,1.11),'root')
bone('spine',(0,0,1.11),(0,0,1.485),'pelvis')
bone('neck',(0,0,1.485),(0,0,1.635),'spine')
bone('head',(0,0,1.635),(0,0,1.86),'neck')
for side,sign in [('L',1),('R',-1)]:
    bone('upper_arm.'+side,(sign*.19,0,1.465),(sign*.425,0,1.465),'spine')
    bone('forearm.'+side,(sign*.425,0,1.465),(sign*.62,0,1.465),'upper_arm.'+side)
    bone('hand.'+side,(sign*.62,0,1.465),(sign*.735,0,1.465),'forearm.'+side)
    bone('thigh.'+side,(sign*.073,0,1.025),(sign*.092,0,.52),'pelvis')
    bone('shin.'+side,(sign*.092,0,.52),(sign*.119,0,.105),'thigh.'+side)
    bone('foot.'+side,(sign*.119,0,.105),(sign*.135,-.115,.035),'shin.'+side)
bpy.ops.object.mode_set(mode='OBJECT')
modifier=mesh.modifiers.new('ResidentSkin','ARMATURE');modifier.object=rig;mesh.parent=rig
rig.animation_data_create()
for clip,duration in [('idle',90),('walk',24)]:
    action=bpy.data.actions.new(clip);rig.animation_data.action=action
    for frame in range(duration+1):
        phase=math.tau*frame/duration
        for pb in rig.pose.bones:
            pb.rotation_mode='XYZ';pb.rotation_euler=(0,0,0);pb.location=(0,0,0)
        for side,sign in [('L',1),('R',-1)]:
            rig.pose.bones['upper_arm.'+side].rotation_euler.z=-sign*math.pi/2
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
rig.animation_data.action=None;scene.frame_set(0)
for pb in rig.pose.bones:pb.rotation_euler=(0,0,0);pb.location=(0,0,0)
mesh['concept']='Low-Poly Character Turnaround Sheet (2).png'
mesh['triangle_budget']=500
mesh['rest_pose']='T-pose'
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
