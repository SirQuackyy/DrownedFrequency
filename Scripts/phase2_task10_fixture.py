import unreal
lib = unreal.EditorAssetLibrary
path = '/Game/Game/Tests/BP_TestInteractionTarget'
if lib.does_asset_exist(path):
    bp = lib.load_asset(path)
else:
    factory = unreal.BlueprintFactory()
    factory.set_editor_property('parent_class', unreal.load_class(None, '/Script/DrownedFrequency.TDFInteractionTestTarget'))
    bp = unreal.AssetToolsHelpers.get_asset_tools().create_asset('BP_TestInteractionTarget', '/Game/Game/Tests', unreal.Blueprint, factory)
assert bp
unreal.BlueprintEditorLibrary.compile_blueprint(bp)
assert lib.save_loaded_asset(bp)
unreal.log('TDF TASK 10: Dummy interactable Blueprint created and compiled.')
