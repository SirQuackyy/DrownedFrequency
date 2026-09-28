import unreal
import toolset_registry
from editor_toolset.toolsets.blueprint import BlueprintTools
lib = unreal.EditorAssetLibrary
path = '/Game/Game/World/BP_Door_Base'
if lib.does_asset_exist(path):
    bp = lib.load_asset(path)
else:
    factory = unreal.BlueprintFactory()
    factory.set_editor_property('parent_class', unreal.load_class(None, '/Script/DrownedFrequency.TDFDoorActor'))
    bp = unreal.AssetToolsHelpers.get_asset_tools().create_asset('BP_Door_Base', '/Game/Game/World', unreal.Blueprint, factory)
unreal.BlueprintEditorLibrary.compile_blueprint(bp)
cdo = unreal.get_default_object(bp.generated_class())
mesh = cdo.get_editor_property('door_mesh')
mesh.set_static_mesh(lib.load_asset('/Engine/BasicShapes/Cube'))
mesh.set_relative_location(unreal.Vector(0, 65, 130), False, False)
mesh.set_relative_scale3d(unreal.Vector(.12, 1.3, 2.6))
mesh.set_material(0, lib.load_asset('/Game/Game/Materials/Blockout/MI_Blockout_Trim'))
graph = BlueprintTools.get_graph(bp, 'EventGraph')
with toolset_registry.tool_raising_exceptions():
    BlueprintTools.add_event(bp, 'OnDoorStateChanged', unreal.IntPoint(0,0))
    BlueprintTools.write_graph_dsl(graph, '''
(event Presentation|EventOnDoorStateChanged (bOpen)
  (Actor|Tick|SetActorTickEnabled :bEnabled true))
(event EventTick (DeltaSeconds)
  (bind hinge (Variables|Door|GetHinge))
  (bind target (select (Variables|Door|GetIsOpen) (Variables|Presentation|GetOpenAngle) 0.0))
  (bind rotation (Math|Interpolation|RInterptoConstant
    :Current (Class|SceneComponent|GetRelativeRotation :self hinge)
    :Target (Math|Rotator|MakeRotator :Yaw target)
    :DeltaTime DeltaSeconds :InterpSpeed (Variables|Presentation|GetSwingSpeed)))
  (Transformation|SetRelativeRotation :self hinge :NewRotation rotation)
  (if (== (.yaw rotation) target)
    (Door|FinishDoorAnimation)
    (Actor|Tick|SetActorTickEnabled :bEnabled false)))
''')
    BlueprintTools.compile_blueprint(bp, True)
assert lib.save_loaded_asset(bp)
