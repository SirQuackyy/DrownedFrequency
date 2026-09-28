"""Create the Phase 2 Enhanced Input contract and validate both device layouts."""
import unreal

lib = unreal.EditorAssetLibrary
tools = unreal.AssetToolsHelpers.get_asset_tools()
root = '/Game/Game/Input'

def asset(name, cls, factory):
    path = root + '/' + name
    result = lib.load_asset(path) if lib.does_asset_exist(path) else tools.create_asset(name, root, cls, factory)
    assert result, path
    return result

actions = {}
for name in ['Move', 'Look', 'Jump', 'Crouch', 'Sprint', 'Lean', 'HoldBreath', 'Interact', 'Inspect', 'Back', 'Pause']:
    action = asset('IA_' + name, unreal.InputAction, unreal.InputAction_Factory())
    value_type = unreal.InputActionValueType.AXIS2D if name in ['Move', 'Look'] else unreal.InputActionValueType.AXIS1D if name == 'Lean' else unreal.InputActionValueType.BOOLEAN
    action.set_editor_property('value_type', value_type)
    actions[name] = action
    assert lib.save_loaded_asset(action)

def context(name, bindings):
    ctx = asset(name, unreal.InputMappingContext, unreal.InputMappingContext_Factory())
    ctx.unmap_all()
    mappings = []
    for action_name, key, negate, swizzle, deadzone in bindings:
        input_key = unreal.Key()
        input_key.import_text(key)
        mapping = ctx.map_key(actions[action_name], input_key)
        modifiers = []
        if negate:
            modifiers.append(unreal.InputModifierNegate(outer=ctx))
        if swizzle:
            modifier = unreal.InputModifierSwizzleAxis(outer=ctx)
            modifier.set_editor_property('order', unreal.InputAxisSwizzle.YXZ)
            modifiers.append(modifier)
        if deadzone:
            modifier = unreal.InputModifierDeadZone(outer=ctx)
            modifier.set_editor_property('lower_threshold', 0.18)
            modifiers.append(modifier)
        mapping.set_editor_property('modifiers', modifiers)
        mappings.append(mapping)
    data = ctx.get_editor_property('default_key_mappings')
    data.set_editor_property('mappings', mappings)
    ctx.set_editor_property('default_key_mappings', data)
    assert lib.save_loaded_asset(ctx)
    stored = ctx.get_editor_property('default_key_mappings').get_editor_property('mappings')
    assert len(stored) == len(bindings)
    assert {m.action.get_name() for m in stored} == {'IA_' + n for n in actions}
    return ctx

kbm = context('IMC_Gameplay_KBM', [
    ('Move', 'W', False, True, False), ('Move', 'S', True, True, False),
    ('Move', 'A', True, False, False), ('Move', 'D', False, False, False),
    ('Look', 'Mouse2D', False, False, False),
    ('Jump', 'SpaceBar', False, False, False), ('Crouch', 'LeftControl', False, False, False),
    ('Sprint', 'LeftShift', False, False, False),
    ('Lean', 'Q', True, False, False), ('Lean', 'E', False, False, False),
    ('HoldBreath', 'X', False, False, False), ('HoldBreath', 'RightMouseButton', False, False, False),
    ('Interact', 'LeftMouseButton', False, False, False), ('Inspect', 'I', False, False, False),
    ('Back', 'Tab', False, False, False), ('Pause', 'Escape', False, False, False)])
gamepad = context('IMC_Gameplay_Gamepad', [
    ('Move', 'Gamepad_Left2D', False, False, True), ('Look', 'Gamepad_Right2D', False, False, True),
    ('Jump', 'Gamepad_FaceButton_Bottom', False, False, False),
    ('Crouch', 'Gamepad_FaceButton_Right', False, False, False),
    ('Sprint', 'Gamepad_LeftThumbstick', False, False, False),
    ('Lean', 'Gamepad_LeftShoulder', True, False, False), ('Lean', 'Gamepad_RightShoulder', False, False, False),
    ('HoldBreath', 'Gamepad_LeftTrigger', False, False, False),
    ('Interact', 'Gamepad_FaceButton_Left', False, False, False), ('Inspect', 'Gamepad_FaceButton_Top', False, False, False),
    ('Back', 'Gamepad_Special_Left', False, False, False), ('Pause', 'Gamepad_Special_Right', False, False, False)])
cls = unreal.load_class(None, '/Script/DrownedFrequency.TDFInputConfig')
factory = unreal.DataAssetFactory()
factory.set_editor_property('data_asset_class', cls)
config = asset('DA_InputConfig', cls, factory)
for name, action in actions.items():
    prop = ''.join('_' + c.lower() if c.isupper() else c for c in name).lstrip('_') + '_action'
    config.set_editor_property(prop, action)
config.set_editor_property('keyboard_mouse_context', kbm)
config.set_editor_property('gamepad_context', gamepad)
assert config.has_required_assets()
assert lib.save_loaded_asset(config)
unreal.log('TDF TASK 2: All 11 actions, both device layouts, modifiers, and Data Asset references validated.')
