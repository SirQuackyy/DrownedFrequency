"""Authored Phase3 route. Reuses actors by label to preserve saved GUIDs on rerun."""
import unreal
lib=unreal.EditorAssetLibrary
levels=unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actors=unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
worlds=unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
path='/Game/Game/Maps/Test/M_Test_PlayerInteraction'
assert levels.load_level(path) if lib.does_asset_exist(path) else levels.new_level(path)
tag='TDF_Phase3Route'
existing={a.get_actor_label():a for a in actors.get_all_level_actors() if tag in [str(t) for t in a.tags]}
used=set()
cube=lib.load_asset('/Engine/BasicShapes/Cube')
wall=lib.load_asset('/Game/Game/Materials/Blockout/MI_Blockout_Walls')
trim=lib.load_asset('/Game/Game/Materials/Blockout/MI_Blockout_Trim')
floor=lib.load_asset('/Game/Game/Materials/Blockout/MI_Blockout_Floor')
def spawn(cls,label,pos,rot=(0,0,0)):
    a=existing.get(label)
    if not a: a=actors.spawn_actor_from_class(cls,unreal.Vector(*pos),unreal.Rotator(pitch=rot[0],yaw=rot[1],roll=rot[2]))
    a.set_actor_location(unreal.Vector(*pos),False,False)
    a.set_actor_rotation(unreal.Rotator(pitch=rot[0],yaw=rot[1],roll=rot[2]),False)
    a.set_actor_label(label); a.set_editor_property('tags',[tag,label]); a.set_folder_path('Phase 3 / '+label.split('_')[0])
    used.add(label)
    return a
def box(label,pos,size,material=wall):
    a=spawn(unreal.StaticMeshActor,label,pos)
    a.static_mesh_component.set_static_mesh(cube)
    a.static_mesh_component.set_material(0,material)
    a.set_actor_scale3d(unreal.Vector(*(v/100 for v in size)))
    return a
def light(label,pos,rot,color,power,radius=650,cone=36):
    a=spawn(unreal.SpotLight,label,pos,rot)
    c=a.get_component_by_class(unreal.SpotLightComponent)
    c.set_mobility(unreal.ComponentMobility.MOVABLE)
    c.set_editor_property('intensity_units',unreal.LightUnits.LUMENS)
    c.set_intensity(power); c.set_light_color(unreal.LinearColor(*color,1))
    c.set_attenuation_radius(radius); c.set_inner_cone_angle(cone*.5); c.set_outer_cone_angle(cone)
    c.set_editor_property('volumetric_scattering_intensity',1.)
    c.set_editor_property('source_radius',3.)
    return a
def sign(label,text,pos,yaw,size=16):
    a=spawn(unreal.TextRenderActor,label,pos,(0,yaw,0))
    c=a.get_component_by_class(unreal.TextRenderComponent)
    c.set_horizontal_alignment(unreal.HorizTextAligment.EHTA_CENTER)
    c.set_text(text); c.set_world_size(size); c.set_text_render_color(unreal.Color(135,149,142,255))
    return a
def spill(label,pos,power,radius=900):
    a=spawn(unreal.RectLight,label,pos,(-90,0,0))
    c=a.get_component_by_class(unreal.RectLightComponent)
    c.set_mobility(unreal.ComponentMobility.MOVABLE)
    c.set_editor_property('intensity_units',unreal.LightUnits.LUMENS)
    c.set_intensity(power); c.set_light_color(unreal.LinearColor(.39,.49,.46,1))
    c.set_attenuation_radius(radius); c.set_source_width(180); c.set_source_height(35)
    c.set_editor_property('volumetric_scattering_intensity',.2)
mode=lib.load_asset('/Game/Game/Core/BP_TDFGameMode').generated_class()
worlds.get_editor_world().get_world_settings().set_editor_property('default_game_mode',mode)
spawn(unreal.PlayerStart,'Route_Start',(-1100,0,100))
box('Shell_Floor',(0,0,-25),(2800,2000,50),floor)
box('Shell_Ceiling',(0,0,365),(2840,2040,30),trim)
for y in [-1000,1000]: box('Shell_Side_'+str(y),(0,y,175),(2840,40,350))
for x in [-1400,1400]: box('Shell_End_'+str(x),(x,0,175),(40,2000,350))
for x in [-1200,-800,-400,0,400,800,1200]:
    box('Rib_Ceiling_'+str(x),(x,0,335),(25,2000,30),trim)
    for y in [-970,970]: box('Rib_Wall_'+str(x)+'_'+str(y),(x,y,170),(25,45,340),trim)
    box('Duct_'+str(x),(x,-220,310),(360,52,45),trim)
    box('Duct_Hanger_'+str(x),(x,-220,340),(12,12,40),trim)
# Solid partitions make the key lock and switch gate meaningful.
for x,label in [(300,'Lock'),(950,'Exit')]:
    box('Partition_'+label+'_North',(x,532.5,175),(35,935,350))
    box('Partition_'+label+'_South',(x,-532.5,175),(35,935,350))
    box('Partition_'+label+'_Header',(x,0,305),(35,130,90),trim)
    for y in [-80,80]: box('Frame_'+label+'_'+str(y),(x-5,y,135),(50,22,270),trim)
# Maintenance pocket: the only entrance is 125 cm tall.
box('Crawl_WestWall',(-860,720,175),(30,530,350))
box('Crawl_EastWall',(-20,720,175),(30,530,350))
box('Crawl_FrontLeft',(-757.5,450,175),(175,30,350))
box('Crawl_FrontRight',(-217.5,450,175),(365,30,350))
box('Crawl_Overhang',(-535,450,237.5),(270,55,225),trim)
box('Crawl_KeyBench',(-170,785,58),(180,100,116),trim)
sign('Sign_Crawl','SERVICE ACCESS / LOW CLEARANCE',(-535,414,180),-90,12)
sign('Sign_Key','B-04 / NORTH ARRAY',(-50,815,195),180,12)
# Broken sightline and inspection alcove.
box('Lean_Corner',(-150,-160,150),(35,430,300),trim)
box('Lean_Return',(-260,-360,150),(250,35,300),trim)
box('Relay_Desk',(-550,-650,58),(230,110,116),trim)
for x in [-1000,-780]:
    box('Service_Cabinet_'+str(x),(x,-900,135),(130,140,270),trim)
    for z in [80,150,220]: box('Service_Slot_'+str(x)+'_'+str(z),(x,-825,z),(90,10,5))
for y in [-800,-720,-640,-560,-480]: box('Grille_'+str(y),(-1100,y,215),(12,9,240),trim)
sign('Sign_Lock','B-04  /  BASEMENT',(278,420,225),180,17)
sign('Sign_LockNote','SERVICE KEY REQUIRED',(277,420,192),180,11)
sign('Sign_Relay','DISCONNECTED AT 02:13',(-550,-710,165),90,12)
sign('Sign_Exit','ARRAY ISOLATION',(928,330,225),180,15)
sign('Sign_Return','DO NOT FOLLOW THE KNOCKING',(1365,210,220),180,12)
door_cls=lib.load_asset('/Game/Game/World/BP_Door_Base').generated_class()
locked=spawn(door_cls,'Interaction_LockedDoor',(300,-65,0))
locked.set_editor_property('is_locked',True); locked.set_editor_property('starts_open',False); locked.set_editor_property('required_key_id','BasementKey')
exit_door=spawn(door_cls,'Interaction_SwitchDoor',(950,-65,0))
exit_door.set_editor_property('is_locked',False); exit_door.set_editor_property('starts_open',False)
item_cls=lib.load_asset('/Game/Game/World/BP_WorldItem_Base').generated_class()
key=spawn(item_cls,'Interaction_BasementKey',(-170,765,124))
key.set_editor_property('item_data',lib.load_asset('/Game/Game/Inventory/DA_Item_BasementKey'))
relay=spawn(item_cls,'Interaction_Relay',(-550,-650,132))
relay.set_editor_property('item_data',lib.load_asset('/Game/Game/Inventory/DA_Item_ArrayRelay'))
relay.set_actor_scale3d(unreal.Vector(2,2,2))
switch=spawn(lib.load_asset('/Game/Game/World/BP_Switch_Base').generated_class(),'Interaction_ExitSwitch',(690,-410,145),(0,0,0))
switch.set_editor_property('linked_actors',[exit_door]); switch.set_editor_property('one_shot',False)
box('Switch_Pedestal',(710,-410,75),(55,70,150),trim)
sign('Sign_Switch','ISOLATION GATE',(725,-410,205),180,12)
# Desaturated utility pools, red beyond the locked threshold, hard occlusion shadows.
light('Light_Entry',(-1160,80,305),(-65,0,0),(.45,.59,.55),2800,730,42)
light('Light_Relay',(-530,-660,290),(-90,0,0),(.65,.57,.40),1400,400,29)
light('Light_Crawl',(-535,600,200),(-35,-90,0),(.42,.56,.50),900,550,40)
light('Light_Key',(-170,765,285),(-90,0,0),(.65,.58,.42),1250,380,28)
light('Light_Bulkhead',(80,-160,315),(-62,40,0),(.47,.60,.56),2600,640,35)
light('Light_Grille',(-1250,-920,300),(-20,40,0),(.26,.38,.49),2100,1250,24)
light('Light_Switch',(710,-410,290),(-85,180,0),(.45,.60,.54),1600,520,34)
light('Light_ExitThreshold',(1150,60,285),(-62,180,0),(.7,.065,.028),1800,650,32)
light('Light_Distant',(1360,-400,310),(-25,130,0),(.25,.39,.49),1900,800,22)
spill('Light_EntrySpill',(-780,100,310),1500)
spill('Light_CornerSpill',(-350,-280,305),1100,750)
spill('Light_ThresholdSpill',(580,120,315),900,650)
for label,pos in [('Entry',(-1160,80,330)),('Relay',(-530,-660,315)),('Key',(-170,765,310)),('Bulkhead',(80,-160,340)),('Switch',(710,-410,315))]:
    box('Fixture_'+label,pos,(65,28,12),trim)
fog=spawn(unreal.ExponentialHeightFog,'Atmosphere_Haze',(0,0,-30))
fc=fog.get_component_by_class(unreal.ExponentialHeightFogComponent)
fc.set_fog_density(.035); fc.set_fog_height_falloff(.8)
fc.set_editor_property('enable_volumetric_fog',True)
fc.set_editor_property('volumetric_fog_extinction_scale',.8)
fc.set_editor_property('volumetric_fog_scattering_distribution',.3)
pp=spawn(unreal.PostProcessVolume,'Atmosphere_Grade',(0,0,0)); pp.set_editor_property('unbound',True)
s=pp.get_editor_property('settings')
for k,v in {'auto_exposure_min_brightness':3.,'auto_exposure_max_brightness':3.,'auto_exposure_bias':0.,
    'bloom_intensity':.2,'vignette_intensity':.32,'color_saturation':unreal.Vector4(.70,.70,.70,1.),
    'motion_blur_amount':0.,'scene_fringe_intensity':0.}.items():
    s.set_editor_property('override_'+k,True); s.set_editor_property(k,v)
pp.set_editor_property('settings',s)
for label,a in existing.items():
    if label not in used: actors.destroy_actor(a)
ids={}
for a in [locked,exit_door,key,relay,switch]:
    comp=a.get_editor_property('stable_identity')
    guid=comp.get_editor_property('stable_actor_id').export_text()
    assert comp.get_editor_property('persist_in_save'),a.get_actor_label()
    assert guid not in ids,(a.get_actor_label(),ids.get(guid))
    ids[guid]=a.get_actor_label()
assert levels.save_current_level()
assert levels.load_level(path)
for a in actors.get_all_level_actors():
    if a.get_actor_label() in ids.values():
        assert a.get_editor_property('stable_identity').get_editor_property('stable_actor_id').export_text() in ids
unreal.EditorLevelLibrary.set_level_viewport_camera_info(unreal.Vector(-1100,0,167),unreal.Rotator(pitch=-2,yaw=5,roll=0))
unreal.log('TDF PHASE3 ROUTE SAVED / GUIDS VERIFIED AFTER RELOAD')
