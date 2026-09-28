import unreal
import json
surface = unreal.EditorAssetLibrary.load_asset('/Engine/BasicShapes/BasicShapeMaterial')
params = unreal.MaterialEditingLibrary.get_vector_parameter_names(surface)
result = {'material': surface.get_path_name(), 'vectors': [str(p) for p in params]}
for p in params:
    result[str(p)] = str(unreal.MaterialEditingLibrary.get_material_default_vector_parameter_value(surface, p))
for a in unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors():
    if isinstance(a, unreal.PostProcessVolume):
        s = a.get_editor_property('settings')
        result['postprocess'] = {p: str(s.get_editor_property(p)) for p in ['auto_exposure_method', 'auto_exposure_min_brightness', 'auto_exposure_max_brightness', 'auto_exposure_bias']}
    if a.get_actor_label() == 'Lighting_ColdOverhead':
        c = a.get_component_by_class(unreal.RectLightComponent)
        result['light'] = {'rotation': str(a.get_actor_rotation()), 'units': str(c.get_editor_property('intensity_units')), 'intensity': c.get_editor_property('intensity')}
unreal.EditorLevelLibrary.set_level_viewport_camera_info(unreal.Vector(-650, -580, 167), unreal.Rotator(pitch=-3, yaw=32, roll=0))
with open(unreal.Paths.project_saved_dir() + 'Logs/Phase2VisualSettings.json', 'w') as f:
    json.dump(result, f, indent=2)
