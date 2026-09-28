"""Low-key station lighting and sightline blockout; no final modeled assets.
Only our authored Phase2 lights and Phase3 ambience actors are replaced.
"""
import unreal
lib = unreal.EditorAssetLibrary
levels = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
tag = 'TDF_Phase3Atmosphere'
cube = lib.load_asset('/Engine/BasicShapes/Cube')
trim = lib.load_asset('/Game/Game/Materials/Blockout/MI_Blockout_Trim')
wall = lib.load_asset('/Game/Game/Materials/Blockout/MI_Blockout_Walls')

def spawn(cls, label, pos, rot=(0,0,0)):
    a = actors.spawn_actor_from_class(cls, unreal.Vector(*pos), unreal.Rotator(pitch=rot[0], yaw=rot[1], roll=rot[2]))
    a.set_actor_label(label)
    a.set_editor_property('tags', [tag])
    a.set_folder_path('Station / Atmosphere')
    return a

def block(label, pos, size, dark=True):
    a=spawn(unreal.StaticMeshActor, label, pos)
    a.static_mesh_component.set_static_mesh(cube)
    a.static_mesh_component.set_material(0, trim if dark else wall)
    a.set_actor_scale3d(unreal.Vector(*(x/100 for x in size)))
    return a

def spot(label, pos, rot, color, lumens, radius=650, cone=35):
    a=spawn(unreal.SpotLight, label, pos, rot)
    c=a.get_component_by_class(unreal.SpotLightComponent)
    c.set_mobility(unreal.ComponentMobility.MOVABLE)
    c.set_editor_property('intensity_units', unreal.LightUnits.LUMENS)
    c.set_intensity(lumens)
    c.set_light_color(unreal.LinearColor(*color,1))
    c.set_attenuation_radius(radius)
    c.set_inner_cone_angle(cone*.55)
    c.set_outer_cone_angle(cone)
    c.set_editor_property('volumetric_scattering_intensity', .7)
    c.set_editor_property('source_radius', 4.0)
    return a

def apply(path):
    assert levels.load_level(path)
    for a in list(actors.get_all_level_actors()):
        tags=[str(t) for t in a.tags]
        if tag in tags or ('TDF_Phase2' in tags and isinstance(a, unreal.Light)):
            actors.destroy_actor(a)
            continue
        if 'TDF_Phase2' not in tags:
            continue
        label=a.get_actor_label()
        if isinstance(a,unreal.TextRenderActor):
            c=a.get_component_by_class(unreal.TextRenderComponent)
            c.set_text_render_color(unreal.Color(114,125,121,255))
            if label=='Sign_Station': c.set_text('NORTH ARRAY / SERVICE LEVEL')
            if label=='Sign_Foundation': c.set_text('NO SIGNAL   //   ACCESS RESTRICTED')
            if label=='Sign_Lean': c.set_text('BLIND CORNER')
        if isinstance(a,unreal.ExponentialHeightFog):
            c=a.get_component_by_class(unreal.ExponentialHeightFogComponent)
            c.set_fog_density(.035)
            c.set_fog_height_falloff(.8)
            c.set_editor_property('volumetric_fog_extinction_scale', .7)
            c.set_editor_property('volumetric_fog_scattering_distribution', .35)
        if isinstance(a,unreal.PostProcessVolume):
            s=a.get_editor_property('settings')
            values={'auto_exposure_min_brightness':3.0,'auto_exposure_max_brightness':3.0,
                'auto_exposure_bias':0., 'bloom_intensity':.18,'vignette_intensity':.32,
                'color_saturation':unreal.Vector4(.72,.72,.72,1.),
                'motion_blur_amount':0., 'scene_fringe_intensity':0.}
            for k,v in values.items():
                s.set_editor_property('override_'+k,True)
                s.set_editor_property(k,v)
            a.set_editor_property('settings',s)
    # Sparse practical pools. The intervening darkness is deliberate, not a room-wide fill.
    spot('Utility_Entry',(-650,-260,390),(-70,25,0),(.53,.65,.61),2400,720,38)
    spot('Utility_Central',(130,-240,380),(-75,25,0),(.57,.67,.64),2100,650,32)
    spot('Utility_Control',(790,-70,330),(-65,40,0),(.55,.65,.62),1800,420,34)
    spot('Utility_Crawl',(-340,880,260),(-65,-90,0),(.62,.50,.29),1100,500,35)
    spot('Emergency_BlindCorner',(750,850,300),(-60,-130,0),(.7,.075,.035),1300,600,38)
    spot('Emergency_Threshold',(920,180,270),(-30,180,0),(.52,.065,.028),800,450,35)
    # A hard side shaft breaks up the far wall without turning the whole room blue.
    spot('Storm_Grille',(-850,-900,330),(-15,50,0),(.30,.43,.58),2200,1300,22)
    for i,x in enumerate([-650,150]):
        a=spawn(unreal.RectLight,'Utility_Spill_'+str(i),(x,-50,400),(-90,0,0))
        c=a.get_component_by_class(unreal.RectLightComponent)
        c.set_mobility(unreal.ComponentMobility.MOVABLE)
        c.set_editor_property('intensity_units',unreal.LightUnits.LUMENS)
        c.set_intensity(1300)
        c.set_light_color(unreal.LinearColor(.39,.49,.46,1))
        c.set_source_width(180); c.set_source_height(35); c.set_attenuation_radius(900)
        c.set_editor_property('volumetric_scattering_intensity',.2)
    for x in [-800,-420,0,420,800]:
        block('Overhead_ServiceDuct',(x,-390,385),(300,55,55))
        block('Duct_Hanger',(x,-390,435),(10,12,90))
    # Recessed service bays and grilles occlude long sightlines; central test lane stays clear.
    for x in [-670,-80,580]:
        block('ServiceBay_Bulkhead',(x,-780,175),(38,410,350),False)
        block('ServiceBay_Header',(x+220,-960,310),(420,50,80))
        for y in [-700,-760,-820,-880,-940]:
            block('Grille_ShadowSlat',(x-20,y,245),(12,10,210))
    for y in [-290,290]:
        block('Threshold_Jamb',(830,y,180),(80,48,360))
    block('Threshold_ShadowLintel',(830,0,370),(80,630,45))
    block('Service_Trench_Edge',(0,-450,6),(1800,14,12))
    assert levels.save_current_level()

for path in ['/Game/Game/Maps/Test/L_TDF_InputGym','/Game/Game/Maps/Persistent/L_TDF_Persistent']:
    apply(path)
unreal.log('TDF PHASE3 ATMOSPHERE SAVED')
