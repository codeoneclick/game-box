"""Run with Python 3; checks canonical content and duplicated project membership."""
from pathlib import Path
import json,struct,wave,zlib,xml.etree.ElementTree as ET,re,hashlib
root=Path(__file__).resolve().parent.parent
bundle=root/'gbBundle';by_name={}
for p in bundle.rglob('*'):
    if p.is_file():by_name.setdefault(p.name,[]).append(p)
assets=[p for p in bundle.rglob('*') if p.is_file() and 'rhythm_ship' in p.name]
pbx=(root/'gbDemo/gbDemo.xcodeproj/project.pbxproj').read_text()
for p in assets:
    assert len(by_name[p.name])==1,p.name
    assert f'/* {p.name} */ = {{isa = PBXFileReference' in pbx,p.name
    phase='Sources' if p.suffix=='.metal' else 'Resources'
    assert pbx.count(f'/* {p.name} in {phase} */ =')==3,(p.name,'target membership')
    if p.suffix=='.xml':
        for e in ET.parse(p).iter():
            name=e.get('filename')
            if name:assert name in by_name or (name+'.vert' in by_name and name+'.frag' in by_name),(p.name,name)
for module,prefix in [('gbDemo','gbDemo'),('gbSound','gb_sound')]:
    source=root/module/'sources';project=(root/module/(prefix+'.vcxproj')).read_text();apple=(root/module/(prefix+'.xcodeproj/project.pbxproj')).read_text()
    for p in source.iterdir():
        if not p.is_file() or not (p.name.startswith(('rhythm_','ces_dance_','fake_leaderboard'))):continue
        assert 'sources\\'+p.name in project,p.name
        assert f'/* {p.name} */ = {{isa = PBXFileReference' in apple,p.name
        if p.suffix=='.cpp':
            assert apple.count(f'/* {p.name} in Sources */ =')==3,p.name
            lines=[l for l in apple.splitlines() if f'/* {p.name} in Sources */ =' in l]
            assert all('-fexceptions -frtti' in l for l in lines),(p.name,'Apple compiler flags')
audio=(root/'gbSound/sources/rhythm_audio_transport.cpp').read_text()
assert 'audio_engine::play2d(' not in audio,'Avoid legacy pending-player/cache lifetime races'
assert 'AL_SAMPLE_OFFSET' in audio and 'AL_SOURCE_STATE' in audio,'Apple clock and failure queries'
png=(bundle/'Textures/rhythm_ship_characters.png').read_bytes();w,h=struct.unpack('>II',png[16:24]);assert(w,h)==(256,32)
offset=8;compressed=b''
while offset<len(png):
    n=struct.unpack('>I',png[offset:offset+4])[0];tag=png[offset+4:offset+8]
    if tag==b'IDAT':compressed+=png[offset+8:offset+8+n]
    offset+=12+n
pixels=zlib.decompress(compressed);stride=1+w*4
for y in range(h):
    assert pixels[y*stride]==0
    for cell in range(8):
        for x in [0,1,30,31]:assert pixels[y*stride+1+(cell*32+x)*4+3]==0
with wave.open(str(bundle/'Sounds/rhythm_ship_metronome.wav'),'rb') as wav:
    assert(wav.getnchannels(),wav.getsampwidth(),wav.getframerate(),wav.getnframes())==(1,2,48000,1440000)
track=json.loads((bundle/'Configurations/rhythm_ship/rhythm_ship_track.json').read_text());assert track['bpm']==128 and track['duration']==30
catalog=json.loads((bundle/'Configurations/rhythm_ship/rhythm_ship_catalog.json').read_text())['tracks']
assert len(catalog)==4
hashes=['5bef1859c4e99e61a14382b1957c12da721a89d78d5136f954bbfd5e1c87f96b','d418b4708d7ee87c65300da6c12b1a7510d09cfaa1c86baaa19296db390e054f','47ca495f806ea69ce2ce140a31729a5a052fb3721ebcc695b66aff6ebc517881']
for entry,digest in zip(catalog,hashes):
    t=json.loads((bundle/'Configurations/rhythm_ship'/entry['map']).read_text())
    file=root/'gbResources'/t['audio_file']
    assert hashlib.sha256(file.read_bytes()).hexdigest()==digest,file.name
    assert pbx.count(f'/* {file.name} in Resources */ =')==3,file.name
    beats=t['beats'];assert 0<=beats[0]<beats[-1]<t['duration']
    assert min([b-a for a,b in zip(beats,beats[1:])]+[t['duration']-beats[-1]+beats[0]])>.26
for name in ['solid','character']:
    material=ET.parse(bundle/f'Configurations/game_objects/materials/material.rhythm_ship_{name}.xml').getroot();assert material.get('technique_name')=='ws.base'
    for ext in ['vert','frag']:assert (bundle/f'Shaders/shader_rhythm_ship_{name}.{ext}').is_file()
char=(bundle/'Configurations/game_objects/materials/material.rhythm_ship_character.xml').read_text();assert 'GL_NEAREST' in char and 'GL_LINEAR' not in char
print(f'Rhythm Ship assets and project membership passed: {len(assets)} assets')
