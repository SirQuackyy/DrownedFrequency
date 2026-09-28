import unreal
lib = unreal.EditorAssetLibrary
bp = lib.load_asset('/Game/Game/Core/BP_TDFPlayerController')
config = lib.load_asset('/Game/Game/Input/DA_InputConfig')
assert config.has_required_assets()
unreal.get_default_object(bp.generated_class()).set_editor_property('input_config', config)
unreal.BlueprintEditorLibrary.compile_blueprint(bp)
assert lib.save_loaded_asset(bp)
unreal.log('TDF TASK 3: Controller input configuration assigned and Blueprint compiled.')
