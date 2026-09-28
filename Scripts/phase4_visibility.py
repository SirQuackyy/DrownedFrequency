"""Raise useful visibility without flattening the horror lighting; safe to rerun."""
import unreal
lib=unreal.EditorAssetLibrary
levels=unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actors=unreal.get_editor_subsystem(unreal.EditorActorSubsystem)

def apply(path):
    assert levels.load_level(path)
    all_actors=actors.get_all_level_actors()
    existing={a.get_actor_label():a for a in all_actors}
    for a in all_actors:
        tags=[str(t) for t in a.tags]
        if not any(t in tags for t in ['TDF_Phase2','TDF_Phase3Route','TDF_Phase3Atmosphere','TDF_Phase4Route']): continue
        if isinstance(a,unreal.PostProcessVolume):
            s=a.get_editor_property('settings')
            # 0.7 stops more visibility; retain fixed exposure so darkness never pumps.
            for k,v in {'auto_exposure_min_brightness':2.3,'auto_exposure_max_brightness':2.3,'vignette_intensity':.18}.items():
                s.set_editor_property('override_'+k,True); s.set_editor_property(k,v)
            a.set_editor_property('settings',s)
        if isinstance(a,unreal.ExponentialHeightFog):
            c=a.get_component_by_class(unreal.ExponentialHeightFogComponent)
            c.set_fog_density(.025)
            c.set_editor_property('volumetric_fog_extinction_scale',.6)
    phase3='M_Test_PlayerInteraction' in path
    points=[('Crawl',(-480,740,285),650,650),('Approach',(20,100,310),550,650),('Exit',(1190,-150,305),700,650)] if phase3 else [('Route',(-280,410,340),750,900),('Corner',(670,560,330),650,800)]
    for label,pos,power,radius in points:
        name='P4_Visibility_'+label
        a=existing.get(name) or actors.spawn_actor_from_class(unreal.RectLight,unreal.Vector(*pos),unreal.Rotator(pitch=-90))
        a.set_actor_label(name); a.set_editor_property('tags',['TDF_Phase4Visibility'])
        c=a.get_component_by_class(unreal.RectLightComponent)
        c.set_mobility(unreal.ComponentMobility.MOVABLE)
        c.set_editor_property('intensity_units',unreal.LightUnits.LUMENS)
        c.set_intensity(power); c.set_light_color(unreal.LinearColor(.46,.53,.50,1))
        c.set_source_width(160); c.set_source_height(35); c.set_attenuation_radius(radius)
        c.set_editor_property('volumetric_scattering_intensity',.1)
    assert levels.save_current_level()

for path in ['/Game/Game/Maps/Test/M_Test_PlayerInteraction','/Game/Game/Maps/Test/L_TDF_InputGym','/Game/Game/Maps/Persistent/L_TDF_Persistent']:
    apply(path)
unreal.log('TDF PHASE4 VISIBILITY UPDATED')
