"""Analyze PCM emitted by check-rhythm-audio.ps1 using the runtime dr_mp3 decoder.

Requires Python 3 + NumPy. Original MP3s are read only. --write-maps updates
canonical maps using the verified pulse choices; evidence is written to output.
"""
from pathlib import Path
import argparse, hashlib, json, wave
import numpy as np

ROOT=Path(__file__).resolve().parent.parent
OUT=ROOT/'gbDemo/output/rhythm-check'
CONFIG=ROOT/'gbBundle/Configurations/rhythm_ship'
MANIFEST=CONFIG/'rhythm_ship_music.json'

def analyze(pcm,bpm):
    x=pcm.astype(np.float32).mean(axis=1).reshape(-1,4).mean(axis=1)/32768
    rate=11025;hop=128;n=1024
    frames=np.lib.stride_tricks.sliding_window_view(x,n)[::hop]
    spectrum=np.abs(np.fft.rfft(frames*np.hanning(n),axis=1))
    frequencies=np.fft.rfftfreq(n,1/rate)
    bands=[]
    for lo,hi in [(35,180),(180,2000)]:
        z=np.log1p(spectrum[:,(frequencies>=lo)&(frequencies<hi)]*20)
        flux=np.maximum(np.diff(z,axis=0,prepend=z[:1]),0).mean(axis=1)
        flux=np.maximum(flux-np.convolve(flux,np.ones(43)/43,'same'),0)
        bands.append(flux/(np.sqrt(np.mean(flux**2))+1e-8))
    y=.55*bands[0]+.45*bands[1];times=(np.arange(len(y))*hop+n*.5)/rate
    period=60/bpm;sections=[]
    for start in np.arange(3,times[-1]-15,15):
        phases=np.arange(0,period,.001);strength=[]
        for phase in phases:
            grid=np.arange(phase,times[-1],period);grid=grid[(grid>=start)&(grid<start+15)]
            strength.append(float(np.mean(np.interp(grid,times,y))))
        sections.append({'start':float(start),'phase':float(phases[np.argmax(strength)])})
    return sections

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--write-maps',action='store_true');parser.add_argument('--pcm-dir',type=Path,default=OUT);args=parser.parse_args()
    report=[];catalog=[]
    for entry in json.loads(MANIFEST.read_text())['tracks']:
        id,title,file,bpm,phase=(entry[k] for k in ['id','title','file','bpm','phase'])
        pcm=np.fromfile(args.pcm_dir/(file+'.pcm'),dtype='<i2').reshape(-1,2)
        duration=len(pcm)/44100
        sections=analyze(pcm,bpm)
        beats=np.arange(phase,duration,60/bpm).tolist();omitted=[]
        while duration-beats[-1]+beats[0]<=.26:omitted.append(beats.pop())
        config={'track':id,'audio_file':file,'bpm':bpm,'duration':duration,'offset':0,'beats':beats,'choreography':'phrases'}
        map_file=f'rhythm_ship_{id}.json'
        if args.write_maps:(CONFIG/map_file).write_text(json.dumps(config,indent=2)+'\n')
        catalog.append({'title':title,'map':map_file})
        row={'file':file,'sha256':hashlib.sha256((ROOT/'gbResources'/file).read_bytes()).hexdigest(),
             'rate':44100,'channels':2,'frames':len(pcm),'duration':duration,'bpm':bpm,'phase':phase,
             'section_phases':sections,'omitted_seam_beats':omitted}
        report.append(row);print(json.dumps(row),flush=True)
        # Portable click overlays for human listening. Audio is output-only and
        # never shipped as a substitute for the user's MP3 files.
        for start in [0,int(duration/2),int(duration)-10]:
            snippet=pcm[start*44100:(start+8)*44100].astype(np.float64)*.45
            for t in beats:
                i=round((t-start)*44100)
                if 0<=i<len(snippet)-882:
                    click=np.sin(np.arange(882)*2*np.pi*1600/44100)*np.exp(-np.arange(882)/200)*12000
                    snippet[i:i+882]+=click[:,None]
            with wave.open(str(OUT/f'{id}-overlay-{start}.wav'),'wb') as f:
                f.setnchannels(2);f.setsampwidth(2);f.setframerate(44100);f.writeframes(np.clip(snippet,-32768,32767).astype('<i2').tobytes())
    catalog.append({'title':'Metronome / 128 BPM','map':'rhythm_ship_track.json'})
    if args.write_maps:(CONFIG/'rhythm_ship_catalog.json').write_text(json.dumps({'tracks':catalog},indent=2)+'\n')
    (OUT/'runtime-mp3-analysis.json').write_text(json.dumps(report,indent=2)+'\n')

if __name__=='__main__':main()
