"""Author separate alternate takes from synthesis, never by analyzing a clean clip."""
from pathlib import Path
import runpy, json
import numpy as np
base=runpy.run_path(str(Path(__file__).with_name('generate_phase4_audio.py')))
base['OUT']=None  # Functions retain their module globals; replace the output directory there.
save=base['save']; save.__globals__['OUT']=Path(__file__).resolve().parents[1]/'ContentSource/Audio/Phase5'
save.__globals__['OUT'].mkdir(parents=True,exist_ok=True)
save.__globals__['report']=[]
time,noise,env=base['time'],base['noise'],base['envelope']
for fidelity in ['Degraded','Contaminated']:
    broken=fidelity=='Contaminated'
    for category in ['ServiceBell','Voice','Machine','Footstep','Water','Static','Impact']:
        duration={'ServiceBell':5.8,'Voice':3.5,'Machine':3.6,'Footstep':.62,'Water':3.2,'Static':2.4,'Impact':1.9}[category]
        t=time(duration); a=np.zeros(len(t))
        if category=='ServiceBell':
            for onset,gain in [(0,1),(1.05,.72),(2.7,.9)]:
                u=np.maximum(t-onset,0)
                for f,d,amp in [(181 if broken else 183,1.3,.8),(502 if broken else 495,.9,.5),(747,.55,.18)]:
                    a+=(t>=onset)*gain*amp*np.sin(2*np.pi*f*u+.4*np.sin(2*np.pi*5*u))*env(u,.007,d)
        elif category=='Voice':
            f0=87+9*np.sin(2*np.pi*.5*t)
            phase=2*np.pi*np.cumsum(f0)/base['SR']
            a=sum(np.sin(k*phase)*np.exp(-((k*90-560)/220)**2)/k for k in range(2,22))*np.sin(np.pi*t/duration)**2
        elif category=='Machine': a=(np.sin(2*np.pi*51*t)+.4*np.sin(2*np.pi*103*t))*(.55+.35*np.sin(2*np.pi*5.3*t)**2)*env(t,.1,1.8)
        elif category=='Footstep': a=noise(len(t),140,1200)*env(t,.007,.07)+np.sin(2*np.pi*74*t)*env(t,.002,.06)*.6
        elif category=='Water':
            for onset,f in [(.06,660),(.72,940),(1.72,760),(2.2,1130)]:
                u=np.maximum(t-onset,0); a+=(t>=onset)*np.sin(2*np.pi*(f*u+180*u*u))*env(u,.004,.11)
        elif category=='Static': a=noise(len(t),400,2600)*(.15+.7*np.sin(np.pi*t/duration)**4)
        else: a=(np.sin(2*np.pi*113*t)+.35*np.sin(2*np.pi*326*t))*env(t,.004,.5)
        # Authored alternate-take texture and ghost resonance are baked into separate WAVs.
        a+=noise(len(t),350,2800)*(.055 if broken else .023)*np.sin(np.pi*t/duration)**2
        if broken:
            a+=.12*np.sin(2*np.pi*69*t+.8*np.sin(2*np.pi*.7*t))*np.sin(np.pi*t/duration)**2
            a*=.7+.3*np.sin(2*np.pi*7.3*t)**2
        save('SW_'+category+'_'+fidelity,a,-13 if broken else -12)
out=save.__globals__['OUT']; report=save.__globals__['report']
(out/'audio_manifest.json').write_text(json.dumps(report,indent=2)+'\n')
(out/'PROVENANCE.md').write_text('# Phase 5 alternate acoustic takes\n\nFourteen original offline synthesized mono PCM WAVs, 48 kHz/16-bit. Separate authored degraded and contaminated takes; no clean-clip analysis, runtime filtering, waveform-derived puzzle truth, external samples, or voice cloning. Core rhythmic identity is retained while the alternate takes use narrower partial sets, electrical instability and low pressure-like resonance. Runtime selects a complete authored file from deterministic metadata. Peak headroom and provenance are recorded in audio_manifest.json.\n')
print('Authored',len(report),'Phase 5 alternate takes.')
