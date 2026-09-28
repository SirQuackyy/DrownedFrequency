import unreal
lib=unreal.EditorAssetLibrary
ui=unreal.get_default_object(unreal.UMGToolSet)
def call(name,*args): return ui.call_method(name,args)
path='/Game/Game/UI/WBP_TDFSaveDebugWidget'
w=lib.load_asset(path) if lib.does_asset_exist(path) else call('CreateWidgetBlueprint','/Game/Game/UI','WBP_TDFSaveDebugWidget',unreal.TDFSaveDebugWidget)
existing={str(e.widget_name):e for e in call('GetWidgets',w).widgets}
def add(cls,name,parent): return existing.get(name) or call('AddWidget',w,cls.static_class(),name,parent,-1)
root=next((e.widget for e in existing.values() if e.parent is None),None)
if root is None: root=add(unreal.CanvasPanel,'SaveCanvas',None).widget
b=add(unreal.Border,'SaveBackdrop',root); b.widget.set_brush_color(unreal.LinearColor(.012,.018,.018,.88))
b.slot.set_anchors(unreal.Anchors(unreal.Vector2D(1,0),unreal.Vector2D(1,0))); b.slot.set_alignment(unreal.Vector2D(1,0)); b.slot.set_position(unreal.Vector2D(-28,28)); b.slot.set_auto_size(True)
size=add(unreal.SizeBox,'SaveWidth',b.widget); size.widget.set_width_override(510)
panel=add(unreal.VerticalBox,'SavePanel',size.widget)
for name,text,font_size in [('SaveTitle','ARCHIVE / SYSTEMS TEST',14),('ObjectiveText','',17),('SaveStatus','',13),('SaveHints','F5: save   F9: load   F10: test controls',13)]:
    e=add(unreal.TextBlock,name,panel.widget); e.widget.set_text(text); e.widget.set_auto_wrap_text(True)
    font=e.widget.get_editor_property('font'); font.size=font_size; e.widget.set_font(font)
    e.widget.set_color_and_opacity(unreal.SlateColor(specified_color=unreal.LinearColor(.8,.85,.8,1))); e.slot.set_padding(unreal.Margin(12,4,12,4))
controls=add(unreal.VerticalBox,'SaveControls',panel.widget)
for name,label in [('SaveButton','Save manual slot 0'),('LoadButton','Load manual slot 0'),('DeleteButton','Delete manual slot 0'),('IntroButton','Load Archive 01 from connector'),('RadioButton','Load Radio Room 02 from connector'),('DeathButton','Test death / checkpoint restore')]:
    button=add(unreal.Button,name,controls.widget); button.slot.set_padding(unreal.Margin(12,3,12,3))
    text=add(unreal.TextBlock,name+'Label',button.widget); text.widget.set_text(label)
unreal.BlueprintEditorLibrary.compile_blueprint(w); lib.save_loaded_asset(w,False)
pc=lib.load_asset('/Game/Game/Core/BP_TDFPlayerController'); unreal.get_default_object(pc.generated_class()).set_editor_property('save_debug_widget_class',w.generated_class())
unreal.BlueprintEditorLibrary.compile_blueprint(pc); lib.save_loaded_asset(pc,False)
p=lib.load_asset('/Game/Game/Core/BP_TDFPlayerCharacter'); unreal.BlueprintEditorLibrary.compile_blueprint(p); lib.save_loaded_asset(p,False)
unreal.log('TDF PHASE6 EVENT-DRIVEN OBJECTIVE / SAVE CONTROLS SAVED')
