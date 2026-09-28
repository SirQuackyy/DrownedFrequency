import unreal
import json
from editor_toolset.toolsets.object import ObjectTools

lib = unreal.EditorAssetLibrary
path = '/Game/Game/UI/WBP_InteractionPrompt'
parent = unreal.load_class(None, '/Script/DrownedFrequency.TDFInteractionPromptWidget')
ui = unreal.get_default_object(unreal.UMGToolSet)
def call(name, *args):
    return ui.call_method(name, args)
bp = lib.load_asset(path) if lib.does_asset_exist(path) else call('CreateWidgetBlueprint', '/Game/Game/UI', 'WBP_InteractionPrompt', parent)
assert bp
tree = call('GetWidgets', bp)
root = next((entry.widget for entry in tree.widgets if entry.parent is None), None)
if root is None:
    root = call('AddWidget', bp, unreal.CanvasPanel.static_class(), 'PromptCanvas', None, -1).widget
existing = {str(entry.widget_name): entry for entry in call('GetWidgets', bp).widgets}
panel = existing.get('PromptPanel') or call('AddWidget', bp, unreal.VerticalBox.static_class(), 'PromptPanel', root, -1)
glyph = existing.get('InputLabel') or call('AddWidget', bp, unreal.TextBlock.static_class(), 'InputLabel', panel.widget, -1)
prompt = existing.get('PromptLabel') or call('AddWidget', bp, unreal.TextBlock.static_class(), 'PromptLabel', panel.widget, -1)
assert panel.widget and glyph.widget and prompt.widget, 'Prompt hierarchy incomplete'
schema = {}
for label, info in [('Panel', panel), ('Glyph', glyph), ('Prompt', prompt)]:
    schema[label] = {'widget': ObjectTools.list_properties(info.widget), 'slot': ObjectTools.list_properties(info.slot) if info.slot else None}
with open(unreal.Paths.project_saved_dir() + 'Logs/Phase2WidgetSchema.json', 'w') as output:
    json.dump(schema, output, indent=2)
unreal.log(ObjectTools.get_properties(panel.slot, ['layoutData', 'bAutoSize']))
panel.slot.set_anchors(unreal.Anchors(unreal.Vector2D(.5, .72), unreal.Vector2D(.5, .72)))
panel.slot.set_alignment(unreal.Vector2D(.5, .5))
panel.slot.set_position(unreal.Vector2D(0, 0))
panel.slot.set_auto_size(True)
for info, size, color in [(glyph, 14, unreal.LinearColor(.92, .69, .35, 1)), (prompt, 22, unreal.LinearColor(.92, .95, 1, 1))]:
    unreal.log(ObjectTools.get_properties(info.widget, ['font', 'colorAndOpacity', 'justification']))
    font = info.widget.get_editor_property('font')
    font.set_editor_property('size', size)
    info.widget.set_font(font)
    info.widget.set_color_and_opacity(unreal.SlateColor(specified_color=color))
    info.widget.set_editor_property('justification', unreal.TextJustify.CENTER)
    info.widget.set_shadow_offset(unreal.Vector2D(1, 2))
    info.widget.set_shadow_color_and_opacity(unreal.LinearColor(0, 0, 0, .9))
    info.widget.set_text('')
    info.slot.set_horizontal_alignment(unreal.HorizontalAlignment.H_ALIGN_CENTER)
    info.slot.set_padding(unreal.Margin(8, 3, 8, 3))
unreal.BlueprintEditorLibrary.compile_blueprint(bp)
assert lib.save_loaded_asset(bp)
controller = lib.load_asset('/Game/Game/Core/BP_TDFPlayerController')
unreal.get_default_object(controller.generated_class()).set_editor_property('prompt_widget_class', bp.generated_class())
unreal.BlueprintEditorLibrary.compile_blueprint(controller)
assert lib.save_loaded_asset(controller)
unreal.log('TDF TASK 12: Prompt Blueprint hierarchy and controller reference created.')
