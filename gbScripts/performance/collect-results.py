"""Persist compact measurements (without thousands of raw frames) for review."""
from __future__ import print_function
import os,json,glob,subprocess
root=os.path.abspath(os.path.join(os.path.dirname(__file__),'..','..'))
captures={}
for directory in sorted(glob.glob(os.path.join(root,'gbDemo','output','island-check','perf-core-audit-*'))):
    summary=os.path.join(directory,'summary.json');details=os.path.join(directory,'profile-details.json')
    if not os.path.isfile(summary):continue
    # Windows PowerShell can emit a UTF-8 BOM depending on host/version.
    entry={'summary':json.loads(open(summary,'rb').read().decode('utf-8-sig'))}
    if os.path.isfile(details):
        p=json.load(open(details))
        entry['details']={k:v for k,v in p.items() if k not in ('cpu','gpu','system_blocks')}
        for field in ('cpu','gpu'):
            entry[field]={k:{a:b for a,b in v.items() if a!='blocks'} for k,v in p[field].items()}
        blocks=p['system_blocks'];keys=set(k for b in blocks for k in b['systems'])
        entry['system_mean_ms']={k:sum(b['systems'].get(k,0) for b in blocks)/len(blocks) for k in keys}
    captures[os.path.basename(directory)]=entry
result={'head':subprocess.check_output(['git','rev-parse','HEAD'],cwd=root).decode('ascii').strip(),
        'date':'2026-10-10','hardware':{'cpu':'Intel Core i3-12100F','gpu':'NVIDIA RTX 3060 Ti 8 GiB','driver':'591.86'},
        'system_mean_note':'Existing system log filters block means <=0.05 ms; missing blocks counted as zero, so partly omitted categories have lower-bound means.',
        'captures':captures}
destination=os.path.join(root,'docs','performance','measurements.json')
open(destination,'w').write(json.dumps(result,indent=2,sort_keys=True,separators=(',', ': '))+'\n')
print(destination)
