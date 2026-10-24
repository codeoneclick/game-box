"""Focused signal checks: onset timestamps come from PCM, not an invented BPM grid."""
import numpy as np
from rhythm_audio_accents import extract_accents

rate=44100
rng=np.random.default_rng(412)
x=np.zeros((rate*9,2),dtype=np.float32)
times=[1.0,1.43,1.86,4.03,6.0,6.75]
for t,amplitude in zip(times,[.35,.35,.4,.95,.55,.8]):
    n=int(.10*rate);u=np.arange(n)/rate
    burst=(.6*np.sin(2*np.pi*95*u)+.4*rng.normal(size=n))*np.exp(-u*45)*amplitude
    start=round(t*rate);x[start:start+n,0]+=burst;x[start:start+n,1]+=burst*.9
result=extract_accents((x*18000).astype(np.int16),rate)
detected=[a['time'] for a in result['accents']]
assert detected, 'Actual attacks must create accent candidates'
for t in times:assert min(abs(a-t) for a in detected)<.025,(t,detected)
assert not any(2.2<t<3.9 for t in detected),'Silence must stay a rest'
assert max(result['accents'],key=lambda a:a['strength'])['time']>4
shifted=np.concatenate([np.zeros((round(.137*rate),2),dtype=np.int16),(x*18000).astype(np.int16)])
again=extract_accents(shifted,rate)['accents']
for a in result['accents']:
    assert min(abs(b['time']-a['time']-.137) for b in again)<.018
assert extract_accents(np.zeros((rate*3,2),dtype=np.int16),rate)['accents']==[]
print('Audio accent checks passed: measured attacks, silence, strength and shifted PCM')
