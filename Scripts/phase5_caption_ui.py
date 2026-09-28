import unreal
lib=unreal.EditorAssetLibrary
ui=unreal.get_default_object(unreal.UMGToolSet)
def call(name,*args): return ui.call_method(name,args)
def widget(name,base):
    path='/Game/Game/UI/'+name
    return lib.load_asset(path) if lib.does_asset_exist(path) else call('CreateWidgetBlueprint','/Game/Game/UI',name,base)
stack=widget('WBP_AcousticCaptionStack',unreal.TDFAcousticCaptionWidget)
existing={str(e.widget_name):e for e in call('GetWidgets',stack).widgets}
def add(cls,name,parent): return existing.get(name) or call('AddWidget',stack,cls.static_class(),name,parent,-1)
root=next((e.widget for e in existing.values() if e.parent is None),None)
if root is None: root=add(unreal.CanvasPanel,'CaptionCanvas',None).widget
b=add(unreal.Border,'CaptionBackdrop',root)
b.widget.set_brush_color(unreal.LinearColor(.008,.012,.014,.88))
b.slot.set_anchors(unreal.Anchors(unreal.Vector2D(.5,.75),unreal.Vector2D(.5,.75)))
b.slot.set_alignment(unreal.Vector2D(.5,1)); b.slot.set_auto_size(True)
t=add(unreal.TextBlock,'CaptionText',b.widget)
t.widget.set_text(''); t.widget.set_auto_wrap_text(True)
t.widget.set_editor_property('wrap_text_at',650)
f=t.widget.get_editor_property('font'); f.size=18; t.widget.set_font(f)
t.widget.set_color_and_opacity(unreal.SlateColor(specified_color=unreal.LinearColor(.88,.89,.84,1)))
t.slot.set_padding(unreal.Margin(16,10,16,10))
unreal.BlueprintEditorLibrary.compile_blueprint(stack); lib.save_loaded_asset(stack,False)
pc=lib.load_asset('/Game/Game/Core/BP_TDFPlayerController')
unreal.get_default_object(pc.generated_class()).set_editor_property('caption_widget_class',stack.generated_class())
unreal.BlueprintEditorLibrary.compile_blueprint(pc); lib.save_loaded_asset(pc,False)
for name in ['WBP_AcousticCaptionLine','WBP_RecorderSlot','WBP_AcousticInteractPrompt']:
    w=widget(name,unreal.UserWidget)
    if not call('GetWidgets',w).widgets:
        e=call('AddWidget',w,unreal.TextBlock.static_class(),'DisplayText',None,-1); e.widget.set_text('')
    unreal.BlueprintEditorLibrary.compile_blueprint(w); lib.save_loaded_asset(w,False)
unreal.log('TDF PHASE5 CAPTION UI SAVED')
