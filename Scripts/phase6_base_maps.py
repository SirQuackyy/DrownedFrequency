"""Traditional streamed save fixture; reuses authored actors/IDs on repeat runs."""
import unreal
from pathlib import Path
lib=unreal.EditorAssetLibrary
levels=unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actors=unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
worlds=unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
tools=unreal.AssetToolsHelpers.get_asset_tools()
tag='TDF_Phase6Route'
existing={}
helper=(Path(__file__).parent/'phase4_test_map.py').read_text(encoding='utf-8-sig')
exec('cube='+helper.split('cube=',1)[1].split('worlds.get_editor_world()',1)[0])
def select(path):
    global existing
    assert levels.load_level(path) if lib.does_asset_exist(path) else levels.new_level(path)
    existing={a.get_actor_label():a for a in actors.get_all_level_actors() if tag in [str(t) for t in a.tags]}
    worlds.get_editor_world().get_world_settings().set_editor_property('default_game_mode',lib.load_asset('/Game/Game/Core/BP_TDFGameMode').generated_class())
def sector(name,center):
    path='/Game/Game/Maps/Sectors/L_TDF_Sector_'+name; select(path)
    x=center
    box('Sector_'+name+'_Floor',(x,600,-25),(1200,900,50),floor)
    box('Sector_'+name+'_Ceiling',(x,600,370),(1240,940,40),trim)
    box('Sector_'+name+'_Back',(x,1050,175),(1240,40,350))
    for side in [-600,600]: box('Sector_'+name+'_Side_'+str(side),(x+side,600,175),(40,900,350))
    for side in [-385,385]: box('Sector_'+name+'_Front_'+str(side),(x+side,150,175),(430,35,350))
    for offset in [-350,100,420]:
        light('Sector_'+name+'_Light_'+str(offset),(x+offset,580,320),1300,850)
        box('Sector_'+name+'_Duct_'+str(offset),(x+offset,450,320),(300,45,40),trim)
    if name=='Intro':
        source('Save_Source','ServiceBell',(300,300,145),23)
        box('Save_SourceStand',(300,300,60),(100,100,120),trim)
        door=spawn(lib.load_asset('/Game/Game/World/BP_Door_Base').generated_class(),'Save_Door',(1100,815,0))
        door.set_editor_property('allow_manual_interaction',False)
        receiver('Save_Receiver',(1050,980,145),unreal.TDFAcousticRoute.DIRECT,door)
        sign('Save_RelaySign','ARCHIVE RELAY',(1040,990,225),180,17)
        # Inventory fixture uses the already-authored Phase 3 key.
        key_assets=[p for p in lib.list_assets('/Game/Game',True,False) if p.split('.')[-1]=='DA_TestKey']
        if not key_assets: key_assets=[p for p in lib.list_assets('/Game/Game',True,False) if 'DA_' in p and 'Key' in p]
        assert key_assets,'Missing authored key definition'
        item=spawn(unreal.TDFWorldItemActor,'Save_Key',(550,850,135))
        item.set_editor_property('item_data',lib.load_asset(key_assets[0]))
        item.get_editor_property('mesh').set_static_mesh(cube); item.get_editor_property('mesh').set_relative_scale3d(unreal.Vector(.25,.12,.08))
        box('Save_KeyStand',(550,850,65),(100,70,130),trim)
    else:
        spawn(lib.load_asset('/Game/Game/Acoustics/BP_AcousticSpeaker').generated_class(),'Save_Speaker',(1800,820,145))
        source('Save_RadioStress','Machine',(2180,900,230),47)
        sign('Save_RadioSign','SIGNAL ARCHIVE / RADIO ROOM',(1800,1020,235),-90,22)
    assert levels.save_current_level()
    folder='/Game/Game/Data/Sectors'; assetpath=folder+'/DA_Sector_'+name
    if lib.does_asset_exist(assetpath): asset=lib.load_asset(assetpath)
    else:
        f=unreal.DataAssetFactory(); f.set_editor_property('data_asset_class',unreal.TDFSectorDataAsset)
        asset=tools.create_asset('DA_Sector_'+name,folder,unreal.TDFSectorDataAsset,f)
    asset.set_editor_property('sector_id',name); asset.set_editor_property('level_package_name',path)
    asset.set_editor_property('neighbor_sector_ids',['RadioRoom' if name=='Intro' else 'Intro'])
    lib.save_loaded_asset(asset,False)
sector('Intro',600); sector('RadioRoom',1800)
path='/Game/Game/Maps/Test/M_Test_SaveStreaming'; select(path)
spawn(unreal.PlayerStart,'Save_Start',(200,0,100),(0,45,0))
box('Connector_Floor',(1200,0,-25),(2400,300,50),floor)
box('Connector_Ceiling',(1200,0,370),(2440,340,40),trim)
box('Connector_Back',(1200,-170,175),(2440,40,350))
for x in [0,2400]: box('Connector_End_'+str(x),(x,0,175),(40,340,350))
for x in [200,700,1200,1700,2200]: light('Connector_Light_'+str(x),(x,0,315),1050,750)
sign('Save_IntroSign','ARCHIVE 01',(600,-140,240),90,22)
sign('Save_RadioRoomSign','RADIO ROOM 02',(1800,-140,240),90,22)
fog=spawn(unreal.ExponentialHeightFog,'Save_Haze',(0,0,-50)); c=fog.get_component_by_class(unreal.ExponentialHeightFogComponent)
c.set_fog_density(.025); c.set_fog_height_falloff(.8); c.set_editor_property('enable_volumetric_fog',True); c.set_editor_property('volumetric_fog_extinction_scale',.6)
pp=spawn(unreal.PostProcessVolume,'Save_Grade',(0,0,0)); pp.set_editor_property('unbound',True); s=pp.get_editor_property('settings')
for k,v in {'auto_exposure_min_brightness':2.3,'auto_exposure_max_brightness':2.3,'bloom_intensity':.18,'vignette_intensity':.18,'color_saturation':unreal.Vector4(.76,.76,.76,1.),'motion_blur_amount':0.}.items():
    s.set_editor_property('override_'+k,True); s.set_editor_property(k,v)
pp.set_editor_property('settings',s)
amb=spawn(unreal.AmbientSound,'Save_RoomTone',(1200,0,160)); ac=amb.get_component_by_class(unreal.AudioComponent)
ac.set_sound(lib.load_asset('/Game/Game/Audio/Phase4/SW_StationRoomTone')); ac.set_volume_multiplier(.65); ac.set_editor_property('auto_activate',True)
for persistent in [path,'/Game/Game/Maps/Persistent/L_TDF_Persistent']:
    assert levels.save_current_level(); assert levels.load_level(persistent)
    w=worlds.get_editor_world()
    for name in ['Intro','RadioRoom']:
        package='/Game/Game/Maps/Sectors/L_TDF_Sector_'+name
        stream=unreal.GameplayStatics.get_streaming_level(w,package)
        if stream is None: stream=unreal.EditorLevelUtils.add_level_to_world(w,package,unreal.LevelStreamingDynamic)
        assert stream
        stream.set_editor_property('initially_loaded',False); stream.set_editor_property('initially_visible',False); stream.set_editor_property('should_be_loaded',False); stream.set_editor_property('should_be_visible',False)
    assert lib.save_loaded_asset(w,False)
    assert levels.save_all_dirty_levels()
assert levels.load_level(path)
for name in ['Intro','RadioRoom']:
    assert unreal.GameplayStatics.get_streaming_level(worlds.get_editor_world(),'/Game/Game/Maps/Sectors/L_TDF_Sector_'+name),name
unreal.log('TDF PHASE6 STREAMED FIXTURE / FOUNDATION TASK20 REFERENCES SAVED')
