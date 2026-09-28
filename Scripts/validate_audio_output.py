"""Offline QA of a captured Unreal master mix (never used by gameplay)."""
import sys, wave, json
from pathlib import Path
import numpy as np
path=Path(sys.argv[1])
with wave.open(str(path),'rb') as w:
    channels,rate,width,frames=w.getnchannels(),w.getframerate(),w.getsampwidth(),w.getnframes()
    assert width==2,('Expected PCM16 Unreal export',width)
    samples=np.frombuffer(w.readframes(frames),dtype='<i2').astype(np.float64)/32768
assert len(samples)>rate*channels*5,'Insufficient captured audio'
peak=float(np.abs(samples).max()); rms=float(np.sqrt(np.mean(samples*samples)))
clipped=int(np.count_nonzero(np.abs(samples)>=.999))
assert rms>.0001,'Engine output is silent'
assert clipped==0,'Engine mix clipped'
result=dict(file=str(path),sample_rate=rate,channels=channels,seconds=round(frames/rate,2),peak_db=round(20*np.log10(peak),2),rms_db=round(20*np.log10(rms),2),clipped_samples=clipped,passed=True)
path.with_suffix('.qa.json').write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps(result,indent=2))
