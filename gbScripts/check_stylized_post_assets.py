from __future__ import print_function
import io, os, re, struct, xml.etree.ElementTree as ET
root=os.path.abspath(os.path.join(os.path.dirname(__file__),'..'))
bundle=os.path.join(root,'gbBundle')
assets={}
for folder,dirs,files in os.walk(bundle):
 for name in files: assets[name]=os.path.join(folder,name)
def text(name):
 with io.open(assets[name],encoding='utf-8-sig') as f:return f.read()
def xml(name):return ET.parse(assets[name]).getroot()
transition=xml('transition.island_demo.xml')
available=set();ss_order=[]
for ref in transition.find('ws_techniques'):
 technique=xml(ref.get('filename'));guid=technique.get('guid');assert guid not in available,guid;available.add(guid)
for ref in transition.find('ss_techniques'):
 technique=xml(ref.get('filename'));guid=technique.get('guid');assert guid not in available,guid
 material=xml(technique.find('material').get('filename'));shader=material.find('shader').get('filename')
 assert shader+'.vert' in assets and shader+'.frag' in assets,shader
 for texture in material.find('textures'):
  op=texture.get('render_operation_name','')
  if op:
   source=op.split('.attachment_')[0] if '.attachment_' in op else op.rsplit('.',1)[0]
   assert source in available,(guid,source)
  elif texture.get('filename'):assert texture.get('filename') in assets
 available.add(guid);ss_order.append(guid)
assert ss_order==['ss.ao','ss.ao.blur','ss.ocean_compose','ss.sunshadow','ss.stylized_final','ss.island_ui'],ss_order
for name in ['ws.technique.island_demo.xml','ws.technique.ocean.xml','ws.technique.ocean_foam.xml','ss.technique.ocean_compose.xml','ss.technique.stylized_final.xml']:
 assert xml(name).find('attachments')[0].get('pixel_format')=='115',name
base=xml('ws.technique.island_demo.xml').find('attachments');assert base[1].get('pixel_format')==base[2].get('pixel_format')=='125'
expected=['post_flags','post_color','post_shadow','post_highlight','post_bloom','post_fog','post_ranges','post_dof','post_extra','post_camera','post_display','post_view','post_projection']
metal=text('mtl_stylized_post.metal')
for shader in ['shader_stylized_output','shader_stylized_final','shader_stylized_bright','shader_stylized_blur_h','shader_stylized_blur_v']:
 names=re.findall(r'uniform\s+(?:vec4|mat4)\s+(post_\w+)\s*;',text(shader+'.frag'))
 assert names==expected,(shader,names)
 for prefix in ['vertex_','fragment_']:assert prefix+shader+'(' in metal,(prefix,shader)
final=text('shader_stylized_final.frag')
assert 'stylized_dof(' not in final and 'sampler_03' not in final and 'sampler_04' not in final
assert 'post_ranges.w' not in final and 'post_extra.x' not in final
final_metal=metal[metal.index('fragment half4 fragment_shader_stylized_final('):metal.index('vertex common_v_output_t vertex_shader_stylized_bright(')]
assert 'bloom.sample' not in final_metal and 'bright.sample' not in final_metal
assert 'stylized_dof(' not in metal and 'u.post_ranges.w' not in final_metal
textures=xml('material.ss.stylized_final.xml').find('textures')
assert not any('stylized_blur' in t.get('render_operation_name','') or 'stylized_bright' in t.get('render_operation_name','') for t in textures)
assert {t.get('sampler_index'):t.get('render_operation_name') for t in textures}['4']=='ws.base.attachment_03'
assert {t.get('sampler_index'):t.get('render_operation_name') for t in textures}['5']=='ws.ocean.attachment_01'
assert xml('material.ss.island_ui.xml').find('textures')[0].get('render_operation_name')=='ss.stylized_final.attachment_01'
assert xml('material.ss.output.island_demo.xml').find('shader').get('filename')=='shader_stylized_output'
for stem in ['forester','stonecutter','woodcutter']:
 textures=xml('material.building.'+stem+'.xml').find('textures');mask=[x for x in textures if x.get('sampler_index')=='1'];assert len(mask)==1
 assert mask[0].get('filename')=='building_emissive_mask.png'
with open(assets['building_emissive_mask.png'],'rb') as f:
 header=f.read(33)
 assert header[:8]==b'\x89PNG\r\n\x1a\n'
 width,height,depth,color=struct.unpack('>IIBB',header[16:26]);assert (width,height,depth,color)==(256,256,8,6),(width,height,depth,color)
print('Stylized post assets: pass dependencies, HDR formats, uniform ABI order, shader pairs and RGBA emissive bindings passed')

# Sun maps store RGB24 depth; sRGB quantization corrupts the depth comparison.
for cascade in ['near','far']:
 technique=xml('ws.technique.sun.'+cascade+'.xml')
 assert technique.find('attachments')[0].get('pixel_format')=='70'
 assert technique.get('frame_width')==technique.get('frame_height')=='2048'
for name in ['rigid','skin','resolve']:
 for suffix in ['.vert','.frag']:assert 'shader_sun_shadow_'+name+suffix in assets
 assert 'vertex_shader_sun_shadow_'+name+'(' in text('mtl_sun_shadow.h')
 assert 'fragment_shader_sun_shadow_'+name+'(' in text('mtl_sun_shadow.h')
sun_inputs={t.get('sampler_index'):t.get('render_operation_name') for t in xml('material.ss.sun_shadow.xml').find('textures')}
assert sun_inputs['3']=='ws.sun.near.attachment_01' and sun_inputs['4']=='ws.sun.far.attachment_01'
assert xml('material.ss.stylized_final.xml').find('textures')[0].get('render_operation_name')=='ss.sunshadow.attachment_01'
print('Sun shadow assets: linear depth maps, cascade sizes, bindings and Metal shader pairs passed')
