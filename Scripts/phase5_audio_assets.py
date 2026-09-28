import unreal
from pathlib import Path
lib=unreal.EditorAssetLibrary
tools=unreal.AssetToolsHelpers.get_asset_tools()
source=Path(unreal.Paths.project_dir())/'ContentSource/Audio/Phase5'
tasks=[]
for file in sorted(source.glob('*.wav')):
    t=unreal.AssetImportTask(); t.filename=str(file); t.destination_path='/Game/Game/Audio/Phase5'; t.automated=True; t.replace_existing=True; t.save=True; tasks.append(t)
tools.import_asset_tasks(tasks)
for name in ['ServiceBell','Voice','Machine','Footstep','Water','Static','Impact']:
    a=lib.load_asset('/Game/Game/Audio/Phase4/DA_'+name)
    variants=dict(a.get_editor_property('variants'))
    for label,fidelity in [('Degraded',unreal.TDFAcousticFidelity.DEGRADED),('Contaminated',unreal.TDFAcousticFidelity.CONTAMINATED)]:
        v=unreal.TDFAcousticClipVariant(); sound=lib.load_asset('/Game/Game/Audio/Phase5/SW_'+name+'_'+label)
        sound.set_editor_property('sound_class_object',lib.load_asset('/Game/Game/Audio/Phase4/SC_WorldCues')); lib.save_loaded_asset(sound,False)
        v.set_editor_property('sound',sound); v.set_editor_property('fidelity',fidelity)
        v.set_editor_property('effective_radius',900. if name=='ServiceBell' else 650.)
        caption=unreal.TDFAcousticCaptionLine(); caption.set_editor_property('text',('[Three iron strikes; the signal wavers]' if name=='ServiceBell' else '['+name+' signal; '+label.lower()+' texture]'))
        caption.set_editor_property('duration',min(sound.duration,3.)); v.set_editor_property('captions',[caption]); variants[fidelity]=v
    a.set_editor_property('variants',variants); a.set_editor_property('can_be_contaminated',name in ['Voice','ServiceBell'])
    assert a.has_valid_definition(); lib.save_loaded_asset(a,False)
path='/Game/Game/Audio/Phase5/DA_AcousticProcessingRules'
if lib.does_asset_exist(path): rules=lib.load_asset(path)
else:
    f=unreal.DataAssetFactory(); f.set_editor_property('data_asset_class',unreal.TDFAcousticProcessingRules)
    rules=tools.create_asset('DA_AcousticProcessingRules','/Game/Game/Audio/Phase5',unreal.TDFAcousticProcessingRules,f)
lib.save_loaded_asset(rules,False)
for name,component in [('/Game/Game/Core/BP_TDFPlayerCharacter','recorder_playback'),('/Game/Game/Acoustics/BP_AcousticPuck','playback')]:
    bp=lib.load_asset(name); c=unreal.get_default_object(bp.generated_class()).get_editor_property(component)
    c.set_editor_property('processing_rules',rules); c.set_editor_property('playback_sound_class',lib.load_asset('/Game/Game/Audio/Phase4/SC_RecorderPlayback'))
    unreal.BlueprintEditorLibrary.compile_blueprint(bp); lib.save_loaded_asset(bp,False)
unreal.log('TDF PHASE5 FOURTEEN AUTHORED VARIANTS / PROCESSING RULES SAVED')
