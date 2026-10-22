"""Register Rhythm Ship sources/assets in the existing MSVC and Apple targets."""
from pathlib import Path
import hashlib
import re
ROOT=Path(__file__).resolve().parent.parent
def uid(value):return hashlib.sha1(value.encode()).hexdigest()[:24].upper()
def clone_pbx(project, anchor, name, path, kind=None):
    text=project.read_text(encoding='utf-8')
    if re.search(r'/\* '+re.escape(name)+r' \*/ = \{isa = PBXFileReference',text):return
    lines=text.splitlines(True)
    selected=[line for line in lines if re.search(r'/\* '+re.escape(anchor)+r'(?: in (?:Sources|Resources))? \*/',line)]
    if not selected:raise RuntimeError(f'{project}: missing anchor {anchor}')
    mapping={}
    for line in selected:
        m=re.match(r'\s*([A-F0-9]{24}) /\*',line)
        if m:mapping[m[1]]=uid(str(project.relative_to(ROOT))+name+m[1])
    output=[]
    for line in lines:
        output.append(line)
        if line not in selected:continue
        new=line.replace(anchor,name)
        for old,value in mapping.items():new=new.replace(old,value)
        if 'PBXFileReference' in new:
            new=re.sub(r'path = [^;]+;',f'path = {path};',new)
            if kind:new=re.sub(r'lastKnownFileType = [^;]+;',f'lastKnownFileType = {kind};',new)
        output.append(new)
    project.write_text(''.join(output),encoding='utf-8')
def source(module,files,anchor):
    project=ROOT/module/(('gb_sound' if module=='gbSound' else 'gbDemo')+'.vcxproj')
    for file in files:
        name=file.name;include='sources\\'+name;tag='ClCompile' if file.suffix=='.cpp' else 'ClInclude'
        for p in [project,Path(str(project)+'.filters')]:
            text=p.read_text(encoding='utf-8-sig')
            if include not in text:text=text.replace('</Project>',f'  <ItemGroup><{tag} Include="{include}" /></ItemGroup>\n</Project>')
            if p == project and tag == 'ClCompile':
                text=text.replace(f'<ClCompile Include="{include}" />',f'<ClCompile Include="{include}"><ExceptionHandling>Sync</ExceptionHandling></ClCompile>')
            p.write_text(text,encoding='utf-8')
        pbx=ROOT/module/(('gb_sound' if module=='gbSound' else 'gbDemo')+'.xcodeproj')/'project.pbxproj'
        clone_pbx(pbx,anchor+file.suffix,name,'sources/'+name)
        if file.suffix=='.cpp':
            apple=pbx.read_text(encoding='utf-8')
            def flags(match):
                line=match.group(0)
                if '-fexceptions -frtti' in line:return line
                return line.rsplit('};',1)[0]+'settings = {COMPILER_FLAGS = "-fexceptions -frtti"; }; };'
            apple=re.sub(r'^.*?/\* '+re.escape(name)+r' in Sources \*/ =.*$',flags,apple,flags=re.M)
            pbx.write_text(apple,encoding='utf-8')
source('gbDemo',sorted(set((ROOT/'gbDemo/sources').glob('rhythm_*.cpp'))|set((ROOT/'gbDemo/sources').glob('rhythm_*.h'))|set((ROOT/'gbDemo/sources').glob('fake_leaderboard.*'))|set((ROOT/'gbDemo/sources').glob('ces_dance_*.*'))),'island_demo_scene')
source('gbSound',list((ROOT/'gbSound/sources').glob('rhythm_audio_transport.*')),'audio_engine')
pbx=ROOT/'gbDemo/gbDemo.xcodeproj/project.pbxproj'
for p in (ROOT/'gbBundle').rglob('*'):
    if not p.is_file() or 'rhythm_ship' not in p.name:continue
    suffix=p.suffix
    anchor='mtl_editor_building.metal' if suffix=='.metal' else 'shader_island_sky.vert' if suffix in ['.vert','.frag'] else 'island_sky.xml'
    kind={'.png':'image.png','.wav':'audio.wav','.json':'text.json','.metal':'sourcecode.metal','.vert':'sourcecode.glsl','.frag':'sourcecode.glsl','.xml':'text.xml'}[suffix]
    clone_pbx(pbx,anchor,p.name,'../'+p.relative_to(ROOT).as_posix(),kind)
print('Rhythm Ship project membership updated')
