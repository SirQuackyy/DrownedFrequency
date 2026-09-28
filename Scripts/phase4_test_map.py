"""Playable acoustic spine room. Authored fixtures are reused so IDs remain stable."""
import unreal
lib=unreal.EditorAssetLibrary
levels=unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actors=unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
worlds=unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
path='/Game/Game/Maps/Test/M_Test_AcousticSpine'
assert levels.load_level(path) if lib.does_asset_exist(path) else levels.new_level(path)
tag='TDF_Phase4Route'
existing={a.get_actor_label():a for a in actors.get_all_level_actors() if tag in [str(t) for t in a.tags]}
cube=lib.load_asset('/Engine/BasicShapes/Cube')
wall=lib.load_asset('/Game/Game/Materials/Blockout/MI_Blockout_Walls')
trim=lib.load_asset('/Game/Game/Materials/Blockout/MI_Blockout_Trim')
floor=lib.load_asset('/Game/Game/Materials/Blockout/MI_Blockout_Floor')
def spawn(cls,label,pos,rot=(0,0,0)):
    a=existing.get(label) or actors.spawn_actor_from_class(cls,unreal.Vector(*pos),unreal.Rotator(pitch=rot[0],yaw=rot[1],roll=rot[2]))
    a.set_actor_location(unreal.Vector(*pos),False,False); a.set_actor_rotation(unreal.Rotator(pitch=rot[0],yaw=rot[1],roll=rot[2]),False)
    a.set_actor_label(label); a.set_editor_property('tags',[tag,label]); a.set_folder_path('Acoustic Test / '+label.split('_')[0]); return a
def box(label,pos,size,mat=wall):
    a=spawn(unreal.StaticMeshActor,label,pos); a.static_mesh_component.set_static_mesh(cube); a.static_mesh_component.set_material(0,mat)
    a.set_actor_scale3d(unreal.Vector(*(v/100 for v in size))); return a
def sign(label,text,pos,yaw=180,size=17):
    a=spawn(unreal.TextRenderActor,label,pos,(0,yaw,0)); c=a.get_component_by_class(unreal.TextRenderComponent)
    c.set_text(text); c.set_world_size(size); c.set_horizontal_alignment(unreal.HorizTextAligment.EHTA_CENTER)
    c.set_text_render_color(unreal.Color(175,189,179,255)); return a
def light(label,pos,power,radius=1000,color=(.47,.56,.52)):
    a=spawn(unreal.RectLight,label,pos,(-90,0,0)); c=a.get_component_by_class(unreal.RectLightComponent)
    c.set_mobility(unreal.ComponentMobility.MOVABLE); c.set_editor_property('intensity_units',unreal.LightUnits.LUMENS)
    c.set_intensity(power); c.set_light_color(unreal.LinearColor(*color,1)); c.set_attenuation_radius(radius)
    c.set_source_width(150); c.set_source_height(30); c.set_editor_property('volumetric_scattering_intensity',.15)
def source(label,clip,pos,repeat=0):
    a=spawn(lib.load_asset('/Game/Game/Acoustics/BP_AudioAnomalyActor').generated_class(),label,pos)
    a.set_editor_property('anomaly_data',lib.load_asset('/Game/Game/Audio/Phase4/DA_'+clip))
    a.set_editor_property('auto_emit_on_begin_play',repeat>0); a.set_editor_property('repeat_interval',repeat)
    return a
def receiver(label,pos,route,door):
    a=spawn(lib.load_asset('/Game/Game/Acoustics/BP_AcousticReceiver').generated_class(),label,pos)
    rule=unreal.TDFAcousticReceiverRule()
    category=unreal.GameplayTag(); category.import_text('Acoustic.Category.Bell')
    keys=unreal.GameplayTagContainer(); keys.import_text('(GameplayTags=((TagName="Acoustic.Key.ServiceBell")))')
    rule.set_editor_property('required_category',category); rule.set_editor_property('required_receiver_keys',keys)
    rule.set_editor_property('accepted_routes',set([route])); rule.set_editor_property('minimum_fidelity',unreal.TDFAcousticFidelity.CLEAN)
    a.get_editor_property('receiver').set_editor_property('rules',[rule]); a.set_editor_property('linked_actor',door)
    return a
worlds.get_editor_world().get_world_settings().set_editor_property('default_game_mode',lib.load_asset('/Game/Game/Core/BP_TDFGameMode').generated_class())
spawn(unreal.PlayerStart,'Route_Start',(-1450,-400,100),(0,0,0))
box('Shell_Floor',(0,0,-25),(3600,2200,50),floor); box('Shell_Ceiling',(0,0,370),(3640,2240,40),trim)
for y in [-1100,1100]: box('Shell_Side_'+str(y),(0,y,175),(3640,40,350))
for x in [-1800,1800]: box('Shell_End_'+str(x),(x,0,175),(40,2200,350))
for x in [-1600,-1200,-800,-400,0,400,800,1200,1600]:
    box('Duct_'+str(x),(x,-380,318),(365,55,45),trim)
    box('Rib_'+str(x),(x,0,342),(22,2200,18),trim)
for x,label in [(-100,'Local'),(1100,'Remote')]:
    for y in [-592.5,592.5]: box('Partition_'+label+str(y),(x,y,175),(35,1055,350))
    box('Header_'+label,(x,0,305),(40,130,90),trim)
    for y in [-81,81]: box('Jamb_'+label+str(y),(x,y,135),(45,22,270),trim)
doorclass=lib.load_asset('/Game/Game/World/BP_Door_Base').generated_class()
direct=spawn(doorclass,'Gate_Direct',(-100,-65,0)); direct.set_editor_property('starts_open',False); direct.set_editor_property('is_locked',False)
remote=spawn(doorclass,'Gate_Puck',(1100,-65,0)); remote.set_editor_property('starts_open',False); remote.set_editor_property('is_locked',False)
# Acoustic gates must not be manually opened; configured native gate policy is assigned below.
direct.set_editor_property('allow_manual_interaction',False); remote.set_editor_property('allow_manual_interaction',False)
box('Bell_Pedestal',(-1350,220,59),(100,100,118),trim)
source('Source_ServiceBell','ServiceBell',(-1350,220,144),18)
receiver('Receiver_Direct',(-145,-220,150),unreal.TDFAcousticRoute.DIRECT,direct)
receiver('Receiver_Puck',(1045,490,150),unreal.TDFAcousticRoute.PUCK_PROJECTED,remote)
box('BlindCorner',(500,650,155),(35,720,310),trim)
box('BlindCorner_Return',(650,980,155),(300,35,310),trim)
box('SafeDesk',(-1450,-760,63),(250,100,126),trim)
source('Source_PipeDrip','Water',(1490,-700,230),27)
sign('Sign_Bell','SERVICE BELL / THREE STRIKES',(-1350,270,220),-90,16)
sign('Sign_Local','LOCAL RELAY\nREPLAY THE BELL HERE',(-129,-320,238),180,16)
sign('Sign_Puck','REMOTE RELAY\nPUCK TRANSMISSION ONLY',(1070,490,225),180,16)
sign('Sign_Recovery','REQUIRED SIGNALS CAN BE RECORDED AGAIN',(-1350,290,185),-90,10)
sign('Sign_Exit','NORTH ARRAY / SIGNAL MAINTENANCE',(1767,0,220),180,19)
sign('Sign_Corner','SIGNAL RECEIVER  >',(472,500,205),180,15)
for name,pos,power,radius in [('Entry',(-1450,-250,320),1450,1000),('Bell',(-1350,240,310),1100,600),('Local',(-390,-160,325),1300,900),('Middle',(250,-100,325),1250,1000),('Receiver',(820,550,325),1000,850),('Threshold',(900,-100,325),1100,900),('Exit',(1490,40,325),1050,900)]:
    light('Light_'+name,pos,power,radius)
    box('Fixture_'+name,(pos[0],pos[1],347),(150,30,10),trim)
light('Light_FarEmergency',(1610,-730,310),260,750,(.65,.14,.065))
fog=spawn(unreal.ExponentialHeightFog,'Atmosphere_Haze',(0,0,-50)); c=fog.get_component_by_class(unreal.ExponentialHeightFogComponent)
c.set_fog_density(.025); c.set_fog_height_falloff(.8); c.set_editor_property('enable_volumetric_fog',True); c.set_editor_property('volumetric_fog_extinction_scale',.6)
pp=spawn(unreal.PostProcessVolume,'Atmosphere_Grade',(0,0,0)); pp.set_editor_property('unbound',True); s=pp.get_editor_property('settings')
for k,v in {'auto_exposure_min_brightness':2.3,'auto_exposure_max_brightness':2.3,'auto_exposure_bias':0.,'bloom_intensity':.18,'vignette_intensity':.18,'color_saturation':unreal.Vector4(.76,.76,.76,1.),'motion_blur_amount':0.,'scene_fringe_intensity':0.}.items():
    s.set_editor_property('override_'+k,True); s.set_editor_property(k,v)
pp.set_editor_property('settings',s)
amb=spawn(unreal.AmbientSound,'Audio_RoomTone',(0,0,160)); ac=amb.get_component_by_class(unreal.AudioComponent); ac.set_sound(lib.load_asset('/Game/Game/Audio/Phase4/SW_StationRoomTone')); ac.set_volume_multiplier(.65)
ac.set_editor_property('auto_activate',True)
assert levels.save_current_level()
unreal.log('TDF PHASE4 ACOUSTIC TEST ROOM SAVED')
