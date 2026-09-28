"""Plain stock-material instances for lighting evaluation, not final surface art."""
import unreal

lib = unreal.EditorAssetLibrary
assets = unreal.AssetToolsHelpers.get_asset_tools()
parent = lib.load_asset('/Engine/BasicShapes/BasicShapeMaterial')
def matte(name, color):
    path = '/Game/Game/Materials/Blockout/' + name
    mi = lib.load_asset(path) if lib.does_asset_exist(path) else assets.create_asset(name, '/Game/Game/Materials/Blockout', unreal.MaterialInstanceConstant, unreal.MaterialInstanceConstantFactoryNew())
    unreal.MaterialEditingLibrary.set_material_instance_parent(mi, parent)
    unreal.MaterialEditingLibrary.set_material_instance_vector_parameter_value(mi, 'Color', unreal.LinearColor(*color))
    unreal.MaterialEditingLibrary.update_material_instance(mi)
    assert lib.save_loaded_asset(mi)
    return mi

wall = matte('MI_Blockout_Walls', (.16, .19, .21, 1))
floor = matte('MI_Blockout_Floor', (.08, .10, .12, 1))
trim = matte('MI_Blockout_Trim', (.03, .045, .06, 1))
levels = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
for path in ['/Game/Game/Maps/Test/L_TDF_InputGym', '/Game/Game/Maps/Persistent/L_TDF_Persistent']:
    assert levels.load_level(path)
    for a in actors.get_all_level_actors():
        label = a.get_actor_label()
        if 'TDF_Phase2' not in [str(t) for t in a.tags] and label != 'Foundation_Floor':
            continue
        mesh = a.get_component_by_class(unreal.StaticMeshComponent)
        if mesh:
            material = floor if 'Floor' in label else trim if any(s in label for s in ['Rib', 'CrossBeam', 'BaseRail', 'Portal', 'Pedestal', 'LightHousing', 'Ceiling']) else wall
            mesh.set_material(0, material)
        if isinstance(a, unreal.RectLight):
            a.get_component_by_class(unreal.RectLightComponent).set_attenuation_radius(900)
        if isinstance(a, unreal.PostProcessVolume):
            s = a.get_editor_property('settings')
            for key in ['auto_exposure_min_brightness', 'auto_exposure_max_brightness']:
                s.set_editor_property(key, 3.0)
            a.set_editor_property('settings', s)
    assert levels.save_current_level()
unreal.EditorLevelLibrary.set_level_viewport_camera_info(unreal.Vector(-650, -580, 167), unreal.Rotator(pitch=-3, yaw=32, roll=0))
unreal.log('TDF VISUAL TUNE: stock blockout instances and lighting contrast saved.')
