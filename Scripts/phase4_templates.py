import unreal
lib=unreal.EditorAssetLibrary
tools=unreal.AssetToolsHelpers.get_asset_tools()
def data(name,cls):
    folder='/Game/Game/Audio/Phase4'; path=folder+'/'+name
    if lib.does_asset_exist(path): return lib.load_asset(path)
    f=unreal.DataAssetFactory(); f.set_editor_property('data_asset_class',cls)
    return tools.create_asset(name,folder,cls,f)
def bp(name,cls):
    folder='/Game/Game/Acoustics'; path=folder+'/'+name
    if lib.does_asset_exist(path): return lib.load_asset(path)
    f=unreal.BlueprintFactory(); f.set_editor_property('parent_class',cls)
    return tools.create_asset(name,folder,unreal.Blueprint,f)
def tag(name):
    t=unreal.GameplayTag(); t.import_text(name); return t
tuning=data('DA_PlayerNoiseTuning',unreal.TDFPlayerNoiseTuning)
entries={}
for cat,sound,radius in [('Voice','Voice',950),('Footstep','Footstep',650),('Machine','Machine',900),('Bell','ServiceBell',1500),('Water','Water',600),('Static','Static',650),('Impact','Impact',1000)]:
    v=unreal.TDFAcousticClipVariant(); v.set_editor_property('sound',lib.load_asset('/Game/Game/Audio/Phase4/SW_'+sound+'_Clean'))
    v.set_editor_property('effective_radius',radius); v.set_editor_property('loudness',.65 if cat=='Footstep' else .6)
    entries[tag('Acoustic.Category.'+cat)]=v
tuning.set_editor_property('categories',entries); assert tuning.has_valid_definition(); lib.save_loaded_asset(tuning,False)
safety=data('DA_AcousticPuzzleSafetyRules',unreal.TDFAcousticPuzzleSafetyRules)
keys=unreal.GameplayTagContainer(); keys.import_text('(GameplayTags=((TagName="Acoustic.Key.ServiceBell")))')
safety.set_editor_property('allowed_fallback_keys',keys); lib.save_loaded_asset(safety,False)
for name in ['/Game/Game/Core/BP_TDFPlayerCharacter','/Game/Game/World/BP_Door_Base','/Game/Game/World/BP_Switch_Base']:
    asset=lib.load_asset(name)
    if not asset: raise RuntimeError('Missing '+name)
    cdo=unreal.get_default_object(asset.generated_class())
    cdo.get_editor_property('noise_emitter').set_editor_property('tuning',tuning)
    if 'PlayerCharacter' in name: cdo.get_editor_property('acoustic_ledger').set_editor_property('safety_rules',safety)
    unreal.BlueprintEditorLibrary.compile_blueprint(asset); lib.save_loaded_asset(asset,False)
for name,cls,component,mesh,scale in [
    ('BP_AudioAnomalyActor',unreal.TDFAudioAnomalyActor,'source_mesh','Cube',(.28,.4,.38)),
    ('BP_AcousticPuck',unreal.TDFAcousticPuckActor,'puck_mesh','Cylinder',(.16,.16,.06)),
    ('BP_AcousticReceiver',unreal.TDFAcousticReceiverActor,'receiver_mesh','Cube',(.12,.32,.40))]:
    asset=bp(name,cls); cdo=unreal.get_default_object(asset.generated_class()); c=cdo.get_editor_property(component)
    c.set_static_mesh(lib.load_asset('/Engine/BasicShapes/'+mesh)); c.set_relative_scale3d(unreal.Vector(*scale))
    c.set_material(0,lib.load_asset('/Game/Game/Materials/Blockout/MI_Blockout_Trim'))
    unreal.BlueprintEditorLibrary.compile_blueprint(asset); lib.save_loaded_asset(asset,False)
import toolset_registry
from editor_toolset.toolsets.blueprint import BlueprintTools
asset=lib.load_asset('/Game/Game/Acoustics/BP_AcousticReceiver')
with toolset_registry.tool_raising_exceptions():
    BlueprintTools.add_event(asset,'OnReceiverVisualChanged',unreal.IntPoint(0,0))
    graph=BlueprintTools.get_graph(asset,'EventGraph')
    BlueprintTools.write_graph_dsl(graph, '''
(event Presentation|EventOnReceiverVisualChanged (bSatisfied)
  (Rendering|SetVisibility :self (Variables|Presentation|GetStatusLight) :bNewVisibility bSatisfied))
''')
    BlueprintTools.compile_blueprint(asset,True)
lib.save_loaded_asset(asset,False)
for name in ['SC_RecorderPlayback','SC_StationAmbience','SC_WorldCues']:
    path='/Game/Game/Audio/Phase4/'+name
    if not lib.does_asset_exist(path): tools.create_asset(name,'/Game/Game/Audio/Phase4',unreal.SoundClass,unreal.SoundClassFactory())
    lib.save_asset(path,False)
for name in ['ServiceBell','Voice','Footstep','Machine','Water','Static','Impact']:
    sound=lib.load_asset('/Game/Game/Audio/Phase4/SW_'+name+'_Clean')
    sound.set_editor_property('sound_class_object',lib.load_asset('/Game/Game/Audio/Phase4/SC_WorldCues'))
    lib.save_loaded_asset(sound,False)
room=lib.load_asset('/Game/Game/Audio/Phase4/SW_StationRoomTone')
room.set_editor_property('sound_class_object',lib.load_asset('/Game/Game/Audio/Phase4/SC_StationAmbience')); lib.save_loaded_asset(room,False)
unreal.log('TDF PHASE4 TEMPLATES SAVED')
