import unreal
lib=unreal.EditorAssetLibrary
tools=unreal.AssetToolsHelpers.get_asset_tools()
folder='/Game/Game/Audio/Phase5'
path=folder+'/DA_ContaminationRules'
if lib.does_asset_exist(path): rules=lib.load_asset(path)
else:
    f=unreal.DataAssetFactory(); f.set_editor_property('data_asset_class',unreal.TDFContaminationRules)
    rules=tools.create_asset('DA_ContaminationRules',folder,unreal.TDFContaminationRules,f)
rules.set_editor_property('overdub_contamination',.25); rules.set_editor_property('unlock_act',5); lib.save_loaded_asset(rules,False)
template_path=folder+'/DA_AudioAnomaly_Template'
template=lib.load_asset(template_path) if lib.does_asset_exist(template_path) else lib.duplicate_asset('/Game/Game/Audio/Phase4/DA_Static',template_path)
sid=unreal.PrimaryAssetId(); sid.import_text('TDFAnomaly:TemplateStatic')
template.set_editor_property('stable_id',sid); template.set_editor_property('display_name','Unidentified interference')
assert template.has_valid_definition(); lib.save_loaded_asset(template,False)
for name,cls,comp,shape,scale in [('BP_AcousticPipeInput',unreal.TDFAcousticPipeNode,'node_mesh','Cylinder',(.3,.3,.5)),('BP_AcousticPipeExit',unreal.TDFAcousticPipeNode,'node_mesh','Cylinder',(.3,.3,.5)),('BP_AcousticSpeaker',unreal.TDFAcousticSpeakerActor,'speaker_mesh','Cube',(.32,.45,.6))]:
    p='/Game/Game/Acoustics/'+name
    if lib.does_asset_exist(p): b=lib.load_asset(p)
    else:
        f=unreal.BlueprintFactory(); f.set_editor_property('parent_class',cls)
        b=tools.create_asset(name,'/Game/Game/Acoustics',unreal.Blueprint,f)
    cdo=unreal.get_default_object(b.generated_class()); mesh=cdo.get_editor_property(comp)
    mesh.set_static_mesh(lib.load_asset('/Engine/BasicShapes/'+shape)); mesh.set_relative_scale3d(unreal.Vector(*scale))
    mesh.set_material(0,lib.load_asset('/Game/Game/Materials/Blockout/MI_Blockout_Trim'))
    if 'Pipe' in name:
        cdo.set_editor_property('is_input',name.endswith('Input')); cdo.set_editor_property('is_output',name.endswith('Exit'))
    else:
        playback=cdo.get_editor_property('playback')
        playback.set_editor_property('processing_rules',lib.load_asset(folder+'/DA_AcousticProcessingRules'))
        playback.set_editor_property('playback_sound_class',lib.load_asset('/Game/Game/Audio/Phase4/SC_RecorderPlayback'))
    unreal.BlueprintEditorLibrary.compile_blueprint(b); lib.save_loaded_asset(b,False)
tuning=lib.load_asset('/Game/Game/Audio/Phase4/DA_PlayerNoiseTuning')
entries=dict(tuning.get_editor_property('categories'))
labels={'Voice':'A muffled voice breaks through.','Footstep':'Footsteps on wet concrete.','Machine':'Metal strains in the walls.','Bell':'Three uneven iron strikes.','Water':'Water drips nearby.','Static':'A burst of interference.','Impact':'A heavy impact echoes.'}
for tag,v in entries.items():
    category=str(tag.get_editor_property('tag_name')).split('.')[-1]
    line=unreal.TDFAcousticCaptionLine(); line.set_editor_property('text',labels[category]); line.set_editor_property('duration',1.4 if category=='Footstep' else 2.5)
    v.set_editor_property('captions',[line]); entries[tag]=v
tuning.set_editor_property('categories',entries); assert len(entries)==7 and tuning.has_valid_definition(); lib.save_loaded_asset(tuning,False)
for p in lib.list_assets('/Game/Game/Audio',True,False):
    a=lib.load_asset(p)
    if isinstance(a,unreal.TDFAudioAnomalyDataAsset): assert a.has_valid_definition(),p
unreal.log('TDF PHASE5 TEMPLATES VALIDATED')
