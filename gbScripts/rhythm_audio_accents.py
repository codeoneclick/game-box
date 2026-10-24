"""Extract acoustic attacks; no metronomic times or choreography templates."""
import numpy as np

def extract_accents(pcm,sample_rate):
    if sample_rate<8000 or sample_rate>192000:raise ValueError('Invalid PCM sample rate')
    audio=np.asarray(pcm)
    if audio.ndim!=2 or audio.shape[1] not in (1,2):raise ValueError('Expected mono/stereo frames')
    audio=audio.astype(np.float32)/(32768 if np.issubdtype(audio.dtype,np.integer) else 1)
    if len(audio)<sample_rate//4 or not np.any(audio):return {'accents':[], 'report':{'count':0}}
    factor=max(1,round(sample_rate/11025));rate=sample_rate/factor
    audio=audio[:len(audio)//factor*factor].reshape(-1,factor,audio.shape[1]).mean(axis=1)
    x=audio.mean(axis=1);hop=64;window=512
    frames=np.lib.stride_tricks.sliding_window_view(x,window)[::hop]
    spectrum=np.abs(np.fft.rfft(frames*np.hanning(window),axis=1))
    frequency=np.fft.rfftfreq(window,1/rate)
    bands=[]
    for low,high in [(35,180),(180,1800),(1800,5000)]:
        z=np.log1p(spectrum[:,(frequency>=low)&(frequency<high)]*30)
        flux=np.maximum(np.diff(z,axis=0,prepend=z[:1]),0).mean(axis=1)
        bands.append(flux/(np.sqrt(np.mean(flux*flux))+1e-8))
    bands=np.asarray(bands);novelty=.5*bands[0]+.3*bands[1]+.2*bands[2]
    times=(np.arange(len(novelty))*hop+window*.5)/rate
    span=max(3,round(rate/hop));kernel=np.ones(span)/span
    mean=np.convolve(novelty,kernel,'same')
    deviation=np.sqrt(np.maximum(0,np.convolve(novelty**2,kernel,'same')-mean**2))
    threshold=np.maximum(mean+.45*deviation,np.quantile(novelty,.65)*.65)
    peaks=np.where((novelty[1:-1]>novelty[:-2])&(novelty[1:-1]>=novelty[2:]))[0]+1
    peaks=peaks[novelty[peaks]>threshold[peaks]]
    # Refine each spectral peak to the actual fast energy attack with a 1ms hop.
    rhop=max(1,round(rate*.001));rwindow=max(2,round(rate*.002))
    offsets=np.arange(0,len(x)-rwindow,rhop)
    integral=np.concatenate([[0],np.cumsum(x.astype(np.float64)**2)])
    rms=np.sqrt(np.maximum(0,(integral[offsets+rwindow]-integral[offsets])/rwindow))
    rt=(offsets+rwindow*.5)/rate
    rise=np.maximum(0,rms-np.concatenate([np.zeros(6),rms[:-6]]))
    silence_floor=max(1e-6,float(np.quantile(rms,.95))*.06)
    candidates=[]
    scale=max(1e-7,float(np.quantile(novelty[peaks],.90))) if len(peaks) else 1
    for k in peaks:
        # A centred STFT frame can peak before the audible attack: refine on
        # both sides of its centre, not only in the already silent past.
        lo=np.searchsorted(rt,max(0,times[k]-.035));hi=np.searchsorted(rt,times[k]+.035)
        if hi<=lo:continue
        attack=lo+int(np.argmax(rise[lo:hi]));time=max(0,float(rt[attack]-.003))
        if rise[attack]<=silence_floor*.15:continue
        tail=np.searchsorted(rt,time+.06);body=rms[attack:max(attack+1,tail)]
        if not len(body) or float(body.max())<silence_floor:continue
        strength=float(np.clip(novelty[k]/scale,0,1))
        a=max(0,round((time-.9)*rate));b=max(a+1,round((time-.15)*rate));c=round(time*rate);d=min(len(x),round((time+.30)*rate))
        before=float(np.mean(x[a:b]**2)) if b>a else 0
        after=float(np.mean(x[c:d]**2)) if d>c else 0
        ratio=after/(before+1e-7)
        low=float(bands[0,k]);mid=float(bands[1,k]);high=float(bands[2,k])
        segment=audio[c:d];pan=0.
        if audio.shape[1]==2 and len(segment):
            energy=(segment**2).mean(axis=0);pan=float((energy[1]-energy[0])/(energy.sum()+1e-9))
        drop=ratio>2.5 and strength>.65 and low>high*.9
        kind='drop' if drop else 'rise' if ratio>1.6 and strength>.65 else 'hit'
        if drop:action='SwipeDown'
        elif kind=='rise' or (high>low*1.8 and strength>.8):action='SwipeUp'
        elif strength>.78:action='SwipeRight' if pan>.025 or (abs(pan)<=.025 and mid>low) else 'SwipeLeft'
        else:action='Tap'
        candidates.append({'time':round(time,6),'action':action,'strength':round(strength,5),
                           'kind':kind,'energy_ratio':round(min(100,ratio),4),
                           'pan':round(pan,4),'timbre':round((high-low)/(high+mid+low+1e-8),4),
                           'spectral_time':round(float(times[k]),6)})
    # Nonmaximum suppression prevents multiple frame peaks for the same attack.
    selected=[]
    for accent in sorted(candidates,key=lambda a:-a['strength']):
        if all(abs(accent['time']-a['time'])>=.13 for a in selected):selected.append(accent)
    selected.sort(key=lambda a:a['time'])
    # Neighbouring percussive attacks are a playable Tap series, not random arrows.
    for i,accent in enumerate(selected):
        neighbours=[abs(accent['time']-selected[j]['time']) for j in [i-1,i+1] if 0<=j<len(selected)]
        if neighbours and min(neighbours)<.55 and accent['kind']=='hit':
            # Ordinary repeated drum hits stay easy to reproduce. A directional
            # accent needs a real stereo change, not a track's fixed mix bias.
            movement=accent['pan']-(selected[i-1]['pan'] if i else accent['pan'])
            prior=[a['timbre'] for a in selected[max(0,i-4):i]]
            timbre_change=accent['timbre']-(float(np.median(prior)) if prior else accent['timbre'])
            if accent['strength']>=.85 and abs(timbre_change)>.18:
                accent['action']='SwipeUp' if timbre_change>0 else 'SwipeDown'
            else:accent['action']=('SwipeRight' if movement>0 else 'SwipeLeft') if accent['strength']>=.85 and abs(movement)>=.08 else 'Tap'
    return {'accents':selected,'report':{'count':len(selected),'analysis_rate':rate,
            'analysis_hop_seconds':hop/rate,'refinement_hop_seconds':rhop/rate,
            'drops':sum(a['kind']=='drop' for a in selected),'silence_floor':silence_floor}}
