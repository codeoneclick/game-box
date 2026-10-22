"""Export the supplied Blockbench axe on the existing Woodsman bind rig (no source edits)."""
import bpy,os,sys,json,base64,importlib.util
sys.dont_write_bytecode=True
from mathutils import Vector,Matrix
root=os.path.abspath(sys.argv[sys.argv.index('--')+1] if '--' in sys.argv else '.')
spec=importlib.util.spec_from_file_location('bb',os.path.join(root,'gbScripts/export-buildings.py'));bb=importlib.util.module_from_spec(spec);spec.loader.exec_module(bb)
model=json.load(open(os.path.join(root,'gbResources/axe.bbmodel'),encoding='utf-8-sig'))
# Face UVs refer to the embedded 256px atlas; model canvas metadata says 16.
model['resolution']={'width':model['textures'][0]['width'],'height':model['textures'][0]['height']}
for e in model['elements']:
 for face in e['faces'].values():face.setdefault('texture',0)
bb.SCALE=1.2/32
verts,faces,uvs,*_=bb.geometry(model)
# Derive the attachment from transformed vertices, including Blockbench group
# rotations. Raw element coordinates are no longer sufficient after author edits.
handle_index=next(i for i,e in enumerate(model['elements']) if e['name']=='axe_handle')
handle_vertices=verts[handle_index*8:handle_index*8+8]
assert len(handle_vertices)==8
head_index=next(i for i,e in enumerate(model['elements']) if e['name']=='axe_head')
handle_center=sum(handle_vertices,Vector())/8
head_center=sum(verts[head_index*8:head_index*8+8],Vector())/8
shaft=(handle_vertices[2]-handle_vertices[0]).normalized()
if (head_center-handle_center).dot(shaft)<0:shaft=-shaft
blade=(head_center-handle_center)-shaft*(head_center-handle_center).dot(shaft)
assert blade.length>.001,'Axe head direction missing'
blade.normalize()
source_frame=Matrix((blade,shaft,blade.cross(shaft))).transposed()
down=Vector((0,0,-1));forward=Vector((0,-1,0))
target_frame=Matrix((down,forward,down.cross(forward))).transposed()
orientation=target_frame@source_frame.transposed()
butt=min((v-handle_center).dot(shaft) for v in handle_vertices)
source_grip=handle_center+shaft*(butt+.4)
verts=[orientation@(v-source_grip) for v in verts]
bpy.ops.object.select_all(action='SELECT');bpy.ops.object.delete(use_global=False)
bpy.ops.import_scene.fbx(filepath=os.path.join(root,'gbBundle/Meshes/woodsman.fbx'))
rig=next(o for o in bpy.context.scene.objects if o.type=='ARMATURE')
body=next(o for o in bpy.context.scene.objects if o.type=='MESH')
hand_group=body.vertex_groups['hand_r'].index
hand=[body.matrix_world@v.co for v in body.data.vertices if any(g.group==hand_group and g.weight>.99 for g in v.groups)]
assert hand,'Right hand vertices missing'
grip=Vector([(min(v[i] for v in hand)+max(v[i] for v in hand))*.5 for i in range(3)])
for o in list(bpy.context.scene.objects):
 if o!=rig:bpy.data.objects.remove(o,do_unlink=True)
# Preserve the agreed bind attachment: forward shaft, downward blade, palm grip.
verts=[grip+v for v in verts]
assert sum(v.z for v in verts[head_index*8:head_index*8+8])/8 < grip.z-.05,'Axe head must face down in the bind pose'
data=bpy.data.meshes.new('WoodcutterAxe');data.from_pydata(verts,[],faces);data.update()
mesh=bpy.data.objects.new('WoodcutterAxe',data);bpy.context.scene.collection.objects.link(mesh)
uv=data.uv_layers.new(name='BlockbenchUV')
for poly,coords in zip(data.polygons,uvs):
 for loop,coord in zip(poly.loop_indices,coords):uv.data[loop].uv=coord
mesh.vertex_groups.new(name='hand_r').add(list(range(len(verts))),1,'REPLACE')
mod=mesh.modifiers.new('HandAttachment','ARMATURE');mod.object=rig;mesh.parent=rig
bpy.ops.object.select_all(action='DESELECT');mesh.select_set(True);rig.select_set(True)
bpy.ops.export_scene.fbx(filepath=os.path.join(root,'gbBundle/Meshes/woodcutter_axe.fbx'),use_selection=True,object_types={'ARMATURE','MESH'},axis_forward='-Z',axis_up='Y',add_leaf_bones=False,apply_scale_options='FBX_SCALE_ALL',bake_anim=False,use_mesh_modifiers=False,path_mode='STRIP')
png=base64.b64decode(model['textures'][0]['source'].split(',',1)[1]);open(os.path.join(root,'gbBundle/Textures/woodcutter_axe.png'),'wb').write(png)
data.calc_loop_triangles();print('AXE_EXPORT vertices=%d triangles=%d bones=%d grip=%s'%(len(verts),len(data.loop_triangles),len(rig.data.bones),list(grip)))
