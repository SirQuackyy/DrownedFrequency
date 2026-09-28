import unreal
lib=unreal.EditorAssetLibrary
path='/Game/Game/World/BP_WorldItem_Base'
if lib.does_asset_exist(path): bp=lib.load_asset(path)
else:
    f=unreal.BlueprintFactory()
    f.set_editor_property('parent_class',unreal.load_class(None,'/Script/DrownedFrequency.TDFWorldItemActor'))
    bp=unreal.AssetToolsHelpers.get_asset_tools().create_asset('BP_WorldItem_Base','/Game/Game/World',unreal.Blueprint,f)
unreal.BlueprintEditorLibrary.compile_blueprint(bp)
mesh=unreal.get_default_object(bp.generated_class()).get_editor_property('mesh')
mesh.set_static_mesh(lib.load_asset('/Engine/BasicShapes/Cube'))
mesh.set_relative_scale3d(unreal.Vector(.24,.12,.08))
mesh.set_material(0,lib.load_asset('/Game/Game/Materials/Blockout/MI_Blockout_Walls'))
assert lib.save_loaded_asset(bp)
unreal.log('TDF WORLD ITEM BLUEPRINT SAVED')
