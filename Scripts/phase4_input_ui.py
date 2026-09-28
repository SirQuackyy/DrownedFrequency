import unreal
lib=unreal.EditorAssetLibrary
tools=unreal.AssetToolsHelpers.get_asset_tools()
config=lib.load_asset('/Game/Game/Input/DA_InputConfig')
bindings=[('Record','record_action','R','Gamepad_RightTrigger'),('Playback','playback_action','F','Gamepad_DPad_Up'),('Project','project_action','G','Gamepad_RightThumbstick'),('NextClip','next_clip_action','RightBracket','Gamepad_DPad_Right'),('PreviousClip','previous_clip_action','LeftBracket','Gamepad_DPad_Left'),('DeleteClip','delete_clip_action','Delete','Gamepad_DPad_Down')]
contexts=[config.get_editor_property('keyboard_mouse_context'),config.get_editor_property('gamepad_context')]
for name,prop,kb,pad in bindings:
    path='/Game/Game/Input/IA_'+name
    a=lib.load_asset(path) if lib.does_asset_exist(path) else tools.create_asset('IA_'+name,'/Game/Game/Input',unreal.InputAction,unreal.InputAction_Factory())
    a.set_editor_property('value_type',unreal.InputActionValueType.BOOLEAN)
    lib.save_loaded_asset(a,False); config.set_editor_property(prop,a)
    for ctx,keyname in zip(contexts,[kb,pad]):
        ctx.unmap_all_keys_from_action(a)
        key=unreal.Key(); key.import_text(keyname); ctx.map_key(a,key)
for ctx in contexts:
    keys={}
    for m in ctx.get_editor_property('default_key_mappings').get_editor_property('mappings'):
        key=m.key.export_text(); name=m.action.get_name()
        assert key not in keys or keys[key]==name,(key,name,keys)
        keys[key]=name
    lib.save_loaded_asset(ctx,False)
lib.save_loaded_asset(config,False)

ui=unreal.get_default_object(unreal.UMGToolSet)
def call(name,*args): return ui.call_method(name,args)
path='/Game/Game/UI/WBP_RecorderSlots'
widget=lib.load_asset(path) if lib.does_asset_exist(path) else call('CreateWidgetBlueprint','/Game/Game/UI','WBP_RecorderSlots',unreal.TDFRecorderWidget)
existing={str(e.widget_name):e for e in call('GetWidgets',widget).widgets}
def add(cls,name,parent): return existing.get(name) or call('AddWidget',widget,cls.static_class(),name,parent,-1)
root=next((e.widget for e in existing.values() if e.parent is None),None)
if root is None: root=add(unreal.CanvasPanel,'RecorderCanvas',None).widget
border=add(unreal.Border,'RecorderBackdrop',root)
border.widget.set_brush_color(unreal.LinearColor(.012,.018,.018,.9))
border.slot.set_anchors(unreal.Anchors(unreal.Vector2D(0,1),unreal.Vector2D(0,1)))
border.slot.set_alignment(unreal.Vector2D(0,1)); border.slot.set_position(unreal.Vector2D(30,-26)); border.slot.set_size(unreal.Vector2D(630,230))
panel=add(unreal.VerticalBox,'RecorderPanel',border.widget)
for name,size,color in [('RecorderTitle',13,(.64,.70,.64,1)),('RecorderSlots',16,(.82,.85,.8,1)),('RecorderStatus',13,(.73,.64,.45,1)),('RecorderControls',12,(.64,.68,.64,1))]:
    e=add(unreal.TextBlock,name,panel.widget); e.widget.set_text('')
    font=e.widget.get_editor_property('font'); font.size=size; e.widget.set_font(font)
    e.widget.set_color_and_opacity(unreal.SlateColor(specified_color=unreal.LinearColor(*color)))
    e.widget.set_auto_wrap_text(True)
    e.slot.set_padding(unreal.Margin(12,4,12,3))
unreal.BlueprintEditorLibrary.compile_blueprint(widget); lib.save_loaded_asset(widget,False)
pc=lib.load_asset('/Game/Game/Core/BP_TDFPlayerController')
unreal.get_default_object(pc.generated_class()).set_editor_property('recorder_widget_class',widget.generated_class())
unreal.BlueprintEditorLibrary.compile_blueprint(pc); lib.save_loaded_asset(pc,False)
p=lib.load_asset('/Game/Game/Core/BP_TDFPlayerCharacter')
unreal.get_default_object(p.generated_class()).get_editor_property('recorder_operator').set_editor_property('puck_class',lib.load_asset('/Game/Game/Acoustics/BP_AcousticPuck').generated_class())
unreal.BlueprintEditorLibrary.compile_blueprint(p); lib.save_loaded_asset(p,False)
unreal.log('TDF PHASE4 INPUT / RECORDER WIDGET SAVED')
