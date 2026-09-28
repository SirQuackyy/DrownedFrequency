"""Focused pools of light with readable floors; preserve all gameplay identities."""
import unreal
lib=unreal.EditorAssetLibrary
levels=unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actors=unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
for sector,x in [('Intro',600),('RadioRoom',1800)]:
    assert levels.load_level('/Game/Game/Maps/Sectors/L_TDF_Sector_'+sector)
    for a in actors.get_all_level_actors():
        label=a.get_actor_label()
        for offset,power in [(-350,380),(100,95),(420,210)]:
            if label=='Sector_'+sector+'_Light_'+str(offset):
                c=a.get_component_by_class(unreal.RectLightComponent)
                c.set_intensity(power); c.set_attenuation_radius(620)
                c.set_light_color(unreal.LinearColor(.32,.43,.48,1))
                c.set_source_width(90); c.set_source_height(18)
    label='Sector_'+sector+'_EmergencyGlow'
    a=next((a for a in actors.get_all_level_actors() if a.get_actor_label()==label),None)
    if a is None:
        a=actors.spawn_actor_from_class(unreal.PointLight,unreal.Vector(x+460,890,250))
        a.set_actor_label(label); a.set_folder_path('Save Test / Lighting')
    c=a.get_component_by_class(unreal.PointLightComponent)
    c.set_mobility(unreal.ComponentMobility.MOVABLE)
    c.set_editor_property('intensity_units',unreal.LightUnits.LUMENS)
    c.set_intensity(45); c.set_attenuation_radius(330)
    c.set_light_color(unreal.LinearColor(.8,.18,.055,1))
    assert levels.save_current_level()
assert levels.load_level('/Game/Game/Maps/Test/M_Test_SaveStreaming')
for a in actors.get_all_level_actors():
    if a.get_actor_label().startswith('Connector_Light_'):
        c=a.get_component_by_class(unreal.RectLightComponent)
        c.set_intensity(220); c.set_attenuation_radius(540)
        c.set_light_color(unreal.LinearColor(.30,.42,.48,1))
assert levels.save_current_level()
unreal.log('TDF PHASE6 FOCUSED HORROR LIGHTING SAVED')
