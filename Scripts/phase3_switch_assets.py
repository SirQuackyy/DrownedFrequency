import unreal
import toolset_registry
from editor_toolset.toolsets.blueprint import BlueprintTools
lib=unreal.EditorAssetLibrary
path='/Game/Game/World/BP_Switch_Base'
if lib.does_asset_exist(path):
    bp=lib.load_asset(path)
else:
    f=unreal.BlueprintFactory()
    f.set_editor_property('parent_class',unreal.load_class(None,'/Script/DrownedFrequency.TDFSwitchActor'))
    bp=unreal.AssetToolsHelpers.get_asset_tools().create_asset('BP_Switch_Base','/Game/Game/World',unreal.Blueprint,f)
unreal.BlueprintEditorLibrary.compile_blueprint(bp)
mesh=unreal.get_default_object(bp.generated_class()).get_editor_property('switch_mesh')
mesh.set_static_mesh(lib.load_asset('/Engine/BasicShapes/Cube'))
mesh.set_relative_scale3d(unreal.Vector(.16,.22,.40))
mesh.set_material(0,lib.load_asset('/Game/Game/Materials/Blockout/MI_Blockout_Walls'))
with toolset_registry.tool_raising_exceptions():
    BlueprintTools.add_event(bp,'OnSwitchVisualChanged',unreal.IntPoint(0,0))
    graph=BlueprintTools.get_graph(bp,'EventGraph')
    BlueprintTools.write_graph_dsl(graph, '''
(event Presentation|EventOnSwitchVisualChanged (bOn)
  (Transformation|SetRelativeRotation :self (Variables|Switch|GetSwitchMesh)
    :NewRotation (Math|Rotator|MakeRotator :Pitch (select bOn -30.0 30.0))))
''')
    BlueprintTools.compile_blueprint(bp,True)
assert lib.save_loaded_asset(bp)
unreal.log('TDF SWITCH BLUEPRINT SAVED')
