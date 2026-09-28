import unreal
lib=unreal.EditorAssetLibrary
tools=unreal.AssetToolsHelpers.get_asset_tools()
levels=unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actors=unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
def data(folder,name,cls):
    path=folder+'/'+name
    if lib.does_asset_exist(path): return lib.load_asset(path)
    f=unreal.DataAssetFactory(); f.set_editor_property('data_asset_class',cls)
    return tools.create_asset(name,folder,cls,f)
folder='/Game/Game/Data/Progression'
definitions=[('QA_Record','Record the archive bell.','Find the three-strike source in Archive 01 and hold record.',['QA_Relay']),('QA_Relay','Replay the bell at the archive relay.','Carry the recorded signal to the relay on the far wall.',['QA_Radio']),('QA_Radio','Restore the radio-room relay.','Follow the connector to Radio Room 02 and operate the isolation switch.',[])]
for identity,short,long,nextids in definitions:
    a=data(folder,'DA_OBJ_'+identity,unreal.TDFObjectiveDataAsset)
    for key,value in {'objective_id':identity,'chapter_id':'QA_Archive','short_text':short,'long_text':long,'next_objective_ids':nextids}.items(): a.set_editor_property(key,value)
    lib.save_loaded_asset(a,False)
c=data(folder,'DA_CH_QA_Archive',unreal.TDFChapterDataAsset)
for key,value in {'chapter_id':'QA_Archive','display_name':'Archive / systems test','persistent_map_name':'/Game/Game/Maps/Test/M_Test_SaveStreaming','entry_checkpoint_id':'QA_CP_01_Archive','objective_ids':[d[0] for d in definitions]}.items(): c.set_editor_property(key,value)
lib.save_loaded_asset(c,False)
d=data('/Game/Game/Data/Difficulty','DA_Difficulty_Standard',unreal.TDFDifficultyProfile)
d.set_editor_property('difficulty_id','Standard'); d.set_editor_property('checkpoint_respawn_health',100); lib.save_loaded_asset(d,False)
for sector in ['Intro','RadioRoom']:
    assert levels.load_level('/Game/Game/Maps/Sectors/L_TDF_Sector_'+sector)
    for a in actors.get_all_level_actors():
        if a.get_actor_label()=='Save_Source': a.set_editor_property('objective_id_to_complete','QA_Record')
        if a.get_actor_label()=='Save_Receiver': a.set_editor_property('objective_id_to_complete','QA_Relay')
    if sector=='RadioRoom':
        a=next((a for a in actors.get_all_level_actors() if a.get_actor_label()=='Save_ObjectiveSwitch'),None)
        if not a: a=actors.spawn_actor_from_class(lib.load_asset('/Game/Game/World/BP_Switch_Base').generated_class(),unreal.Vector(1900,750,145))
        a.set_actor_label('Save_ObjectiveSwitch'); a.set_editor_property('tags',['TDF_Phase6Route','Save_ObjectiveSwitch']); a.set_editor_property('one_shot',True); a.set_editor_property('objective_id_to_complete','QA_Radio')
    assert levels.save_current_level()
assert levels.load_level('/Game/Game/Maps/Test/M_Test_SaveStreaming')
for label,x,enter,leave in [('ToIntro',950,['Intro'],['RadioRoom']),('ToRadio',1450,['RadioRoom'],['Intro'])]:
    a=next((a for a in actors.get_all_level_actors() if a.get_actor_label()==label),None)
    if not a: a=actors.spawn_actor_from_class(unreal.TDFSectorVolume,unreal.Vector(x,0,110))
    a.set_actor_label(label); a.set_editor_property('tags',['TDF_Phase6Route',label]); a.set_editor_property('enter_sector_ids',enter); a.set_editor_property('exit_sector_ids',leave)
    a.get_editor_property('trigger').set_box_extent(unreal.Vector(60,125,110),False)
if not any(isinstance(a,unreal.TDFSaveTestBootstrap) for a in actors.get_all_level_actors()):
    a=actors.spawn_actor_from_class(unreal.TDFSaveTestBootstrap,unreal.Vector(0,0,-200)); a.set_actor_label('Save_TestManager')
assert levels.save_current_level()
unreal.log('TDF PHASE6 PROGRESSION / SAFE SECTOR TRANSITIONS SAVED')
