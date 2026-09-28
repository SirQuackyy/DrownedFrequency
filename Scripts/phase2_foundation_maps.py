"""Phase 2 foundation maps: primitive geometry, lighting, and movement fixtures.

Only actors tagged TDF_Phase2 are regenerated. Existing user actors are preserved.
No final art, production interactions, or streamed-sector logic is authored here.
"""
import unreal

lib = unreal.EditorAssetLibrary
levels = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
editor = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
cube = lib.load_asset('/Engine/BasicShapes/Cube')
surface = lib.load_asset('/Engine/BasicShapes/BasicShapeMaterial')
assert surface
mode = lib.load_asset('/Game/Game/Core/BP_TDFGameMode').generated_class()
tag = 'TDF_Phase2'

def spawn(cls, label, pos, rot=(0, 0, 0)):
    a = actors.spawn_actor_from_class(cls, unreal.Vector(*pos), unreal.Rotator(pitch=rot[0], yaw=rot[1], roll=rot[2]))
    assert a, label
    a.set_actor_label(label)
    a.set_editor_property('tags', [tag])
    a.set_folder_path('Player Foundation/' + label.split('_')[0])
    return a

def block(label, pos, dimensions):
    a = spawn(unreal.StaticMeshActor, label, pos)
    a.static_mesh_component.set_static_mesh(cube)
    a.static_mesh_component.set_material(0, surface)
    a.set_actor_scale3d(unreal.Vector(*(v / 100 for v in dimensions)))
    return a

def rect(label, pos, rot, color, intensity, width=180, height=30):
    a = spawn(unreal.RectLight, label, pos, rot)
    c = a.get_component_by_class(unreal.RectLightComponent)
    c.set_mobility(unreal.ComponentMobility.MOVABLE)
    c.set_editor_property('intensity_units', unreal.LightUnits.LUMENS)
    c.set_intensity(intensity)
    c.set_light_color(unreal.LinearColor(*color))
    c.set_source_width(width)
    c.set_source_height(height)
    c.set_attenuation_radius(1400)
    c.set_editor_property('volumetric_scattering_intensity', .6)
    return a

def lettering(label, text, pos, yaw, size=23):
    a = spawn(unreal.TextRenderActor, label, pos, (0, yaw, 0))
    c = a.get_component_by_class(unreal.TextRenderComponent)
    c.set_text(text)
    c.set_world_size(size)
    c.set_text_render_color(unreal.Color(187, 202, 205, 255))

def build_room(path):
    if lib.does_asset_exist(path):
        assert levels.load_level(path)
    else:
        assert levels.new_level(path)
    for a in actors.get_all_level_actors():
        if tag in [str(t) for t in a.tags]:
            actors.destroy_actor(a)
    world = editor.get_editor_world()
    world.get_world_settings().set_editor_property('default_game_mode', mode)
    old = actors.get_all_level_actors()
    for a in old:
        if a.get_actor_label() == 'Foundation_Floor':
            a.static_mesh_component.set_material(0, surface)
    if not any(a.get_actor_label() == 'Foundation_Floor' for a in old):
        block('Architecture_Floor', (0, 0, -25), (2000, 2000, 50))
    if not any(isinstance(a, unreal.PlayerStart) for a in old):
        spawn(unreal.PlayerStart, 'Player_Start', (0, 0, 100))
    # The central 12 m lane stays clear for repeatable movement/input tests.
    for y in [-1000, 1000]:
        block('Architecture_SideWall', (0, y, 250), (2040, 40, 500))
        block('Architecture_BaseRail', (0, y * .96, 35), (1980, 18, 45))
    for x in [-1000, 1000]:
        block('Architecture_EndWall', (x, 0, 250), (40, 2000, 500))
    block('Architecture_Ceiling', (0, 0, 515), (2040, 2040, 30))
    for x in [-800, -400, 0, 400, 800]:
        block('Architecture_CrossBeam', (x, 0, 470), (35, 1980, 65))
        for y in [-950, 950]:
            block('Architecture_Rib', (x, y, 240), (40, 60, 480))
        for y in [-580, 580]:
            block('Architecture_LightHousing', (x + 80, y, 490), (185, 40, 18))
    # Strong cool/warm hierarchy, with luminance from real lights, not emissive hacks.
    for x in [-650, 100, 750]:
        rect('Lighting_ColdOverhead', (x, -450, 455), (-90, 0, 0), (.36, .66, 1, 1), 5000)
        rect('Lighting_AmberOverhead', (x, 680, 450), (-90, 0, 0), (1, .48, .14, 1), 4500)
    rect('Lighting_FarWallWash', (850, 0, 370), (0, 0, 0), (.24, .6, 1, 1), 2400, 600, 100)
    rect('Lighting_EntryBounce', (-800, 0, 300), (-25, 0, 0), (.45, .57, .7, 1), 1000, 300, 100)
    # Portal silhouettes, a lean corner, and a 125 cm high crouch route.
    for y in [-260, 260]:
        block('Fixtures_PortalPost', (850, y, 160), (60, 50, 320))
    block('Fixtures_PortalLintel', (850, 0, 335), (60, 570, 50))
    block('Fixtures_LeanCorner', (480, 700, 145), (35, 350, 290))
    for x in [-520, -160]:
        block('Fixtures_CrouchPillar', (x, 720, 85), (40, 300, 170))
    block('Fixtures_CrouchOverhang', (-340, 720, 152), (400, 300, 54))
    target = spawn(unreal.load_class(None, '/Game/Game/Tests/BP_TestInteractionTarget.BP_TestInteractionTarget_C'),
                   'Fixtures_InteractionTarget', (910, 0, 150))
    target.set_editor_property('prompt', 'Check station control')
    target.get_component_by_class(unreal.StaticMeshComponent).set_material(0, surface)
    block('Fixtures_ControlPedestal', (910, 0, 65), (65, 70, 130))
    lettering('Sign_Station', 'THE DROWNED FREQUENCY', (970, 330, 390), 180, 30)
    lettering('Sign_Foundation', 'PLAYER FOUNDATION  /  INPUT GYM', (969, 335, 350), 180, 14)
    lettering('Sign_Crouch', '01  /  LOW CLEARANCE', (-550, 908, 240), -90, 20)
    lettering('Sign_Lean', '02  /  LEAN & SIGHTLINE', (310, 908, 350), -90, 20)
    fog = spawn(unreal.ExponentialHeightFog, 'Atmosphere_InteriorHaze', (0, 0, -50))
    fc = fog.get_component_by_class(unreal.ExponentialHeightFogComponent)
    fc.set_fog_density(.018)
    fc.set_editor_property('enable_volumetric_fog', True)
    fc.set_editor_property('volumetric_fog_scattering_distribution', .3)
    fc.set_editor_property('volumetric_fog_extinction_scale', .5)
    pp = spawn(unreal.PostProcessVolume, 'Atmosphere_ExposureAndLens', (0, 0, 0))
    pp.set_editor_property('unbound', True)
    settings = pp.get_editor_property('settings')
    values = {'auto_exposure_min_brightness': 4.5, 'auto_exposure_max_brightness': 4.5,
              'auto_exposure_bias': 0.0, 'bloom_intensity': .25, 'vignette_intensity': .22,
              'motion_blur_amount': 0.0, 'scene_fringe_intensity': 0.0}
    for key, value in values.items():
        settings.set_editor_property('override_' + key, True)
        settings.set_editor_property(key, value)
    pp.set_editor_property('settings', settings)
    assert levels.save_current_level()

build_room('/Game/Game/Maps/Test/L_TDF_InputGym')
build_room('/Game/Game/Maps/Persistent/L_TDF_Persistent')
for name in ['L_TDF_Sector_Intro', 'L_TDF_Sector_RadioRoom']:
    path = '/Game/Game/Maps/Sectors/' + name
    if not lib.does_asset_exist(path):
        assert levels.new_level(path)
        editor.get_editor_world().get_world_settings().set_editor_property('default_game_mode', mode)
        assert levels.save_current_level()
for path in ['/Game/Game/Maps/Test/L_TDF_InputGym', '/Game/Game/Maps/Sectors/L_TDF_Sector_Intro',
             '/Game/Game/Maps/Sectors/L_TDF_Sector_RadioRoom', '/Game/Game/Maps/Persistent/L_TDF_Persistent']:
    assert levels.load_level(path), path
unreal.EditorLevelLibrary.set_level_viewport_camera_info(unreal.Vector(-730, -670, 220), unreal.Rotator(pitch=-4, yaw=38, roll=0))
unreal.log('TDF FOUNDATION MAPS: all four maps saved and reopened successfully.')
