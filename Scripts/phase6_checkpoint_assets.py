import unreal
lib=unreal.EditorAssetLibrary
levels=unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actors=unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
tools=unreal.AssetToolsHelpers.get_asset_tools()
path='/Game/Game/World/BP_TDFCheckpointActor'
if lib.does_asset_exist(path): bp=lib.load_asset(path)
else:
    f=unreal.BlueprintFactory(); f.set_editor_property('parent_class',unreal.TDFCheckpointActor)
    bp=tools.create_asset('BP_TDFCheckpointActor','/Game/Game/World',unreal.Blueprint,f)
unreal.BlueprintEditorLibrary.compile_blueprint(bp); lib.save_loaded_asset(bp,False)
for sector,x,checkpoint,start in [('Intro',600,'QA_CP_01_Archive',True),('RadioRoom',1800,'QA_CP_02_Radio',False)]:
    assert levels.load_level('/Game/Game/Maps/Sectors/L_TDF_Sector_'+sector)
    label='Checkpoint_'+sector
    a=next((a for a in actors.get_all_level_actors() if a.get_actor_label()==label),None)
    if a is None: a=actors.spawn_actor_from_class(bp.generated_class(),unreal.Vector(x,220,100))
    a.set_actor_label(label); a.set_editor_property('tags',['TDF_Phase6Route',label])
    a.set_editor_property('checkpoint_id',checkpoint); a.set_editor_property('chapter_id','QA_Archive')
    transform=unreal.Transform(location=unreal.Vector(x,240,92),rotation=unreal.Rotator(0,90,0))
    a.set_editor_property('respawn_transform',transform); a.get_editor_property('respawn_arrow').set_world_transform(transform,False,False)
    a.set_editor_property('required_loaded_sectors',[sector]); a.set_editor_property('chapter_start',start)
    assert levels.save_current_level()
unreal.log('TDF PHASE6 CHECKPOINTS SAVED')
