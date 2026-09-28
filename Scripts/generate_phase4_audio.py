"""Original, deterministic offline sound design. No microphone or downloaded samples.
Outputs authored PCM WAV assets; the game never analyzes audio to solve puzzles.
"""
from pathlib import Path
import json, wave, hashlib
import numpy as np

ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'ContentSource'/'Audio'/'Phase4'
OUT.mkdir(parents=True,exist_ok=True)
SR=48000
rng=np.random.default_rng(42017)
report=[]

def noise(n,lo,hi):
    f=np.fft.rfftfreq(n,1/SR)
    spectrum=np.fft.rfft(rng.normal(0,1,n))
    filt=np.exp(-(f/hi)**4)*(1-np.exp(-(f/max(lo,1))**4))
    a=np.fft.irfft(spectrum*filt,n)
    return a/(np.std(a)+1e-12)

def time(d): return np.arange(round(d*SR))/SR
def envelope(t,attack=.015,decay=1.): return (1-np.exp(-t/attack))*np.exp(-t/decay)
def save(name,a,peak_db=-10,loop=False):
    a=np.asarray(a,dtype=np.float64)
    a-=a.mean(axis=0)
    if not loop:
        n=min(int(.02*SR),len(a)//4)
        env=np.ones(len(a)); env[:n]=np.linspace(0,1,n); env[-n:]=np.linspace(1,0,n)
        a*=env[:,None] if a.ndim==2 else env
    a*=10**(peak_db/20)/max(np.abs(a).max(),1e-9)
    pcm=np.round(a*32767).astype('<i2')
    path=OUT/(name+'.wav')
    with wave.open(str(path),'wb') as w:
        w.setnchannels(1 if a.ndim==1 else a.shape[1]); w.setsampwidth(2); w.setframerate(SR); w.writeframes(pcm.tobytes())
    assert np.isfinite(a).all() and np.abs(a).max()<.8 and np.sqrt(np.mean(a*a))>.0001
    if loop: assert np.max(np.abs(a[0]-a[-1]))<.025, name
    report.append(dict(name=name,seconds=round(len(a)/SR,3),channels=1 if a.ndim==1 else a.shape[1],peak_db=round(20*np.log10(np.abs(a).max()),2),rms_db=round(20*np.log10(np.sqrt(np.mean(a*a))),2),loop=loop,sha256=hashlib.sha256(path.read_bytes()).hexdigest()))

# Signature bell: three unevenly spaced strikes, inharmonic iron partials, long decays.
t=time(5.8); bell=np.zeros(len(t))
for onset,gain in [(0,1),(1.05,.72),(2.7,.9)]:
    u=np.maximum(t-onset,0); gate=t>=onset
    for f,dec,amp in [(183,1.8,1),(497,1.2,.55),(761,.85,.3),(1103,.55,.12)]:
        bell+=gate*gain*amp*np.sin(2*np.pi*f*u)*envelope(u,.004,dec)
save('SW_ServiceBell_Clean',bell,-9)
t=time(3.6)
machine=(np.sin(2*np.pi*53*t)+.35*np.sin(2*np.pi*107*t)+.15*noise(len(t),90,850))*(.7+.2*np.sin(2*np.pi*4.2*t))*envelope(t,.09,2)
save('SW_Machine_Clean',machine,-13)
t=time(.62)
step=(noise(len(t),65,1800)*envelope(t,.004,.065)+.5*np.sin(2*np.pi*(95*t-35*t*t))*envelope(t,.002,.085))
save('SW_Footstep_Clean',step,-15)
t=time(1.9)
impact=sum(np.sin(2*np.pi*f*t)*envelope(t,.003,d)*a for f,d,a in [(117,.65,1),(317,.4,.45),(743,.19,.2)])+.12*noise(len(t),140,4800)*envelope(t,.001,.03)
save('SW_Impact_Clean',impact,-12)
t=time(3.2); water=noise(len(t),300,3800)*.03
for onset,f in [(.05,720),(.68,1100),(1.75,870),(2.18,1320)]:
    u=np.maximum(t-onset,0)
    water+=(t>=onset)*np.sin(2*np.pi*(f*u+230*(1-np.exp(-u*35))/35))*envelope(u,.002,.09)
save('SW_Water_Clean',water,-14)
t=time(2.4)
static=noise(len(t),250,4500)*(.2+.65*np.sin(np.pi*t/2.4)**6)
save('SW_Static_Clean',static,-20)
# Unintelligible exhaled vocal tone, not dialogue or a cloned person's voice.
t=time(3.5); f0=92+8*np.sin(2*np.pi*.6*t); phase=2*np.pi*np.cumsum(f0)/SR
voice=sum(np.sin(k*phase)*np.exp(-((k*96-620)/260)**2)/k for k in range(1,24))
voice=(voice+.025*noise(len(t),500,2300))*np.sin(np.pi*t/3.5)**2
save('SW_Voice_Clean',voice,-17)
# Periodic low-level stereo room tone: no stingers or musical chord progression.
t=time(24)
def room(phase):
    a=.12*np.sin(2*np.pi*41*t+phase)*(1+.2*np.sin(2*np.pi*t/24))
    a+=.06*np.sin(2*np.pi*63.125*t+phase*.3)+.035*np.sin(2*np.pi*126.25*t)
    a+=.075*noise(len(t),65,600)*(1+.35*np.sin(2*np.pi*t/12+phase))
    a+=.012*noise(len(t),1200,3200)
    return a
save('SW_StationRoomTone',np.stack([room(0),room(.4)],axis=1),-22,True)
(OUT/'audio_manifest.json').write_text(json.dumps(report,indent=2)+'\n')
(OUT/'PROVENANCE.md').write_text('# Phase 4 authored test audio\n\nAll eight WAV files are original offline synthesis from generate_phase4_audio.py (fixed seed 42017). No external samples, microphone input, voice cloning, or purchased assets. 48 kHz / 16-bit PCM. Seven localized mono cues; one quiet stereo room-tone loop. Voice is an unintelligible synthetic exhalation, not final dialogue. Clean variants only in Phase 4; later variants must be authored separately. Audio metadata, not waveform content, drives gameplay. Numeric QA in audio_manifest.json checks finite samples, peak headroom, duration, and loop boundary continuity. Human mix/subjective listening approval remains separate.\n')
print(json.dumps(report,indent=2))
