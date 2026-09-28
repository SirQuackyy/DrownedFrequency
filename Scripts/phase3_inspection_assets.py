import unreal
lib=unreal.EditorAssetLibrary
ui=unreal.get_default_object(unreal.UMGToolSet)
def call(name,*args): return ui.call_method(name,args)
path='/Game/Game/UI/WBP_Inspection'
bp=lib.load_asset(path) if lib.does_asset_exist(path) else call('CreateWidgetBlueprint','/Game/Game/UI','WBP_Inspection',unreal.load_class(None,'/Script/DrownedFrequency.TDFInspectionWidget'))
existing={str(e.widget_name):e for e in call('GetWidgets',bp).widgets}
def add(cls,name,parent):
    e=existing.get(name) or call('AddWidget',bp,cls.static_class(),name,parent,-1)
    assert e.widget,name
    return e
root=next((e.widget for e in existing.values() if e.parent is None),None)
if root is None: root=add(unreal.CanvasPanel,'InspectionCanvas',None).widget
bg=add(unreal.Border,'Backdrop',root)
bg.widget.set_brush_color(unreal.LinearColor(.009,.014,.015,1.))
bg.slot.set_anchors(unreal.Anchors(unreal.Vector2D(0,0),unreal.Vector2D(1,1)))
bg.slot.set_offsets(unreal.Margin(0,0,0,0))
image=add(unreal.Image,'PreviewImage',root)
image.slot.set_anchors(unreal.Anchors(unreal.Vector2D(.29,.5),unreal.Vector2D(.29,.5)))
image.slot.set_alignment(unreal.Vector2D(.5,.5))
image.slot.set_position(unreal.Vector2D(0,0))
image.slot.set_size(unreal.Vector2D(450,450))
panel=add(unreal.VerticalBox,'DetailsPanel',root)
panel.slot.set_anchors(unreal.Anchors(unreal.Vector2D(.57,.34),unreal.Vector2D(.57,.34)))
panel.slot.set_position(unreal.Vector2D(0,0))
panel.slot.set_size(unreal.Vector2D(460,400))
for name,size,text,color in [
    ('RecordLabel',13,'NORTH ARRAY / FIELD RECORD',(.45,.55,.52,1)),
    ('ItemTitle',28,'',(.85,.87,.81,1)),
    ('ItemDescription',20,'',(.64,.69,.66,1)),
    ('InspectionControls',15,'',(.65,.53,.34,1))]:
    e=add(unreal.TextBlock,name,panel.widget)
    font=e.widget.get_editor_property('font'); font.set_editor_property('size',size); e.widget.set_font(font)
    e.widget.set_text(text)
    e.widget.set_color_and_opacity(unreal.SlateColor(specified_color=unreal.LinearColor(*color)))
    e.widget.set_auto_wrap_text(True)
    e.slot.set_padding(unreal.Margin(0,0,0,22))
unreal.BlueprintEditorLibrary.compile_blueprint(bp)
assert lib.save_loaded_asset(bp,False)  # Runtime widget setters do not always mark the template package dirty.
pc=lib.load_asset('/Game/Game/Core/BP_TDFPlayerController')
unreal.get_default_object(pc.generated_class()).set_editor_property('inspection_widget_class',bp.generated_class())
unreal.BlueprintEditorLibrary.compile_blueprint(pc)
assert lib.save_loaded_asset(pc)
unreal.log('TDF INSPECTION UI SAVED')
