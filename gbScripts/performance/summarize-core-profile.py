from __future__ import print_function
import re,json,sys,os
def stats(values):
    values=sorted(values)
    if not values:return None
    def percentile(p):
        pos=(len(values)-1)*p;i=int(pos);f=pos-i
        return values[i]*(1-f)+values[min(i+1,len(values)-1)]*f
    return dict(count=len(values),mean=sum(values)/len(values),p50=percentile(.5),p95=percentile(.95),p99=percentile(.99),maximum=values[-1])
for directory in sys.argv[1:]:
    text=open(os.path.join(directory,'runtime.log')).read()
    result={}
    result['system_blocks']=[]
    for chunk in text.split('PROFILE wall_ms=')[1:]:
        match=re.match(r'([\d.]+) feeder_ms=([\d.]+)',chunk)
        block={'wall_ms':float(match.group(1)),'feeder_ms':float(match.group(2)),'systems':{}}
        for name,ms in re.findall(r'^PROFILE (.+) ms=([\d.]+)',chunk,re.M):block['systems'][name]=float(ms)
        result['system_blocks'].append(block)
    result['frame_ms']=stats([float(v) for v in re.findall(r'^PROBE_FRAME frame=\d+ wall_ms=([\d.]+)',text,re.M)])
    result['gpu_frame_ms']=stats([float(v) for v in re.findall(r'^PROBE_GPU frame=\d+ total_ms=([\d.]+)',text,re.M)])
    for field,pattern in [('cpu',r'^PROBE_CPU_AVG name=(\S+) ms_per_frame=([\d.]+) calls_per_frame=([\d.]+)'),('gpu',r'^PROBE_GPU_AVG name=(\S+) ms=([\d.]+) calls=(\d+)')]:
        entries={}
        for name,ms,calls in re.findall(pattern,text,re.M):entries.setdefault(name,[]).append({'ms':float(ms),'calls':float(calls)})
        result[field]={name:dict(mean_ms=sum(v['ms'] for v in values)/len(values),mean_calls=sum(v['calls'] for v in values)/len(values),blocks=values) for name,values in entries.items()}
    result['draw_calls']=stats([float(v) for v in re.findall(r'^PROBE_DRAW frame=\d+ triangles=\d+ calls=(\d+)',text,re.M)])
    result['triangles']=stats([float(v) for v in re.findall(r'^PROBE_DRAW frame=\d+ triangles=(\d+)',text,re.M)])
    uploads=[(int(frame),float(ms)) for frame,ms in re.findall(r'^PROBE_UPLOAD frame=(\d+) ms=([\d.]+)',text,re.M)]
    frames={int(frame):float(ms) for frame,ms in re.findall(r'^PROBE_FRAME frame=(\d+) wall_ms=([\d.]+)',text,re.M)}
    upload_frames={}
    for frame,ms in uploads:upload_frames[frame]=upload_frames.get(frame,0)+ms
    result['upload_call_ms']=stats([ms for frame,ms in uploads])
    result['upload_frame_ms']=stats(list(upload_frames.values()))
    result['frames_after_upload_ms']=stats([frames[frame+1] for frame in upload_frames if frame+1 in frames])
    result['frames_without_upload_ms']=stats([ms for frame,ms in frames.items() if frame-1 not in upload_frames])
    destination=os.path.join(directory,'profile-details.json')
    open(destination,'w').write(json.dumps(result,indent=2,sort_keys=True,separators=(',', ': '))+'\n')
    print(destination)
