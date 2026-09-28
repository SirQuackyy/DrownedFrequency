import unreal
from pathlib import Path
lib=unreal.EditorAssetLibrary
tools=unreal.AssetToolsHelpers.get_asset_tools()
folder='/Game/Game/Audio/Phase4'
source=Path(unreal.Paths.project_dir())/'ContentSource'/'Audio'/'Phase4'
tasks=[]
for file in sorted(source.glob('*.wav')):
    t=unreal.AssetImportTask(); t.filename=str(file); t.destination_path=folder
    t.automated=True; t.replace_existing=True; t.save=True
    tasks.append(t)
tools.import_asset_tasks(tasks)
cls=unreal.load_class(None,'/Script/DrownedFrequency.TDFAudioAnomalyDataAsset')
def tag(name):
    v=unreal.GameplayTag(); v.import_text('(TagName="'+name+'")'); return v
for cat,sound in [('Voice','Voice'),('Footstep','Footstep'),('Machine','Machine'),('Bell','ServiceBell'),('Water','Water'),('Static','Static'),('Impact','Impact')]:
    name='DA_'+sound; path=folder+'/'+name
    if lib.does_asset_exist(path): a=lib.load_asset(path)
    else:
        f=unreal.DataAssetFactory(); f.set_editor_property('data_asset_class',cls)
        a=tools.create_asset(name,folder,cls,f)
    identity=unreal.PrimaryAssetId(); identity.import_text('TDFAnomaly:'+sound)
    a.set_editor_property('stable_id',identity)
    a.set_editor_property('display_name','Service bell / three strikes' if cat=='Bell' else cat+' signal')
    a.set_editor_property('category_tag',tag('Acoustic.Category.'+cat))
    v=unreal.TDFAcousticClipVariant(); v.set_editor_property('sound',lib.load_asset(folder+'/SW_'+sound+'_Clean'))
    v.set_editor_property('effective_radius',900. if cat=='Bell' else 650.)
    caption=unreal.TDFAcousticCaptionLine(); caption.set_editor_property('text',{'Bell':'[Three uneven iron bell strikes]','Machine':'[Motor judders against a loose housing]','Water':'[Water drips into a hollow pipe]','Static':'[A narrow band of radio static]','Impact':'[Metal knocks against the bulkhead]','Footstep':'[A hard-soled footstep]','Voice':'[An indistinct breath in the signal]'}[cat])
    caption.set_editor_property('duration',min(3.,v.get_editor_property('sound').duration))
    v.set_editor_property('captions',[caption]); variants=dict(a.get_editor_property('variants')); variants[unreal.TDFAcousticFidelity.CLEAN]=v; a.set_editor_property('variants',variants)
    keys=unreal.GameplayTagContainer()
    if cat=='Bell': keys.import_text('(GameplayTags=((TagName="Acoustic.Key.ServiceBell")))')
    a.set_editor_property('receiver_keys',keys); a.set_editor_property('can_satisfy_receiver',cat=='Bell')
    assert a.has_valid_definition(),name
    assert lib.save_loaded_asset(a,False)
room=lib.load_asset(folder+'/SW_StationRoomTone')
room.set_editor_property('looping',True); assert lib.save_loaded_asset(room,False)
for t in tasks: assert t.imported_object_paths,str(t.filename)
unreal.log('TDF PHASE4 EIGHT WAVS IMPORTED / SEVEN VALID ANOMALIES')
