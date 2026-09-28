"""Create and validate Phase 2 task 1 presentation shells in Unreal Editor."""
import unreal

assets = unreal.AssetToolsHelpers.get_asset_tools()
library = unreal.EditorAssetLibrary

def blueprint(name, parent):
    path = '/Game/Game/Core/' + name
    if library.does_asset_exist(path):
        return library.load_asset(path)
    factory = unreal.BlueprintFactory()
    factory.set_editor_property('parent_class', unreal.load_class(None, '/Script/DrownedFrequency.' + parent))
    result = assets.create_asset(name, '/Game/Game/Core', unreal.Blueprint, factory)
    assert result, path
    unreal.BlueprintEditorLibrary.compile_blueprint(result)
    assert library.save_loaded_asset(result), path
    return result

pawn = blueprint('BP_TDFPlayerCharacter', 'TDFPlayerCharacter')
controller = blueprint('BP_TDFPlayerController', 'TDFPlayerController')
mode = blueprint('BP_TDFGameMode', 'TDFGameMode')
defaults = unreal.get_default_object(mode.generated_class())
defaults.set_editor_property('default_pawn_class', pawn.generated_class())
defaults.set_editor_property('player_controller_class', controller.generated_class())
unreal.BlueprintEditorLibrary.compile_blueprint(mode)
library.save_loaded_asset(mode)

levels = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
map_path = '/Game/Game/Maps/Test/L_TDF_InputGym'
if not library.does_asset_exist(map_path):
    assert levels.new_level(map_path)
    floor = actors.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(0, 0, -25))
    floor.set_actor_label('Foundation_Floor')
    floor.static_mesh_component.set_static_mesh(library.load_asset('/Engine/BasicShapes/Cube'))
    floor.set_actor_scale3d(unreal.Vector(20, 20, .5))
    actors.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(0, 0, 100))
else:
    assert levels.load_level(map_path)
world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
world.get_world_settings().set_editor_property('default_game_mode', mode.generated_class())
assert levels.save_current_level()
assert defaults.get_editor_property('default_pawn_class') == pawn.generated_class()
assert defaults.get_editor_property('player_controller_class') == controller.generated_class()
unreal.log('TDF TASK 1: Blueprint shells and spawn map validated.')
