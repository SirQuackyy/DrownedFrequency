import unreal
lib=unreal.EditorAssetLibrary
cls=unreal.load_class(None,'/Script/DrownedFrequency.TDFItemDataAsset')
def item(name, values):
    path='/Game/Game/Inventory/'+name
    if lib.does_asset_exist(path): a=lib.load_asset(path)
    else:
        f=unreal.DataAssetFactory()
        f.set_editor_property('data_asset_class',cls)
        a=unreal.AssetToolsHelpers.get_asset_tools().create_asset(name,'/Game/Game/Inventory',cls,f)
    for k,v in values.items(): a.set_editor_property(k,v)
    a.set_editor_property('inspection_material_override',lib.load_asset('/Game/Game/Materials/Blockout/MI_Blockout_Walls'))
    assert a.has_valid_definition(), name
    assert lib.save_loaded_asset(a)
item('DA_Item_BasementKey',dict(item_id='Key.Basement', display_name='Basement service key',
    short_description='Cold brass. The tag reads NORTH ARRAY / B-04.',item_type=unreal.TDFItemType.KEY_ITEM,
    can_pick_up=True,persists_in_inventory=True,can_inspect=True,key_id='BasementKey',
    inspection_mesh=lib.load_asset('/Engine/BasicShapes/Cube')))
item('DA_Item_ArrayRelay',dict(item_id='Evidence.ArrayRelay',display_name='Disconnected relay',
    short_description='Maintenance record: the relay was disconnected at 02:13.\nThe knocking continued after the array lost power.',
    item_type=unreal.TDFItemType.INSPECTABLE,can_pick_up=False,persists_in_inventory=False,can_inspect=True,
    inspection_mesh=lib.load_asset('/Engine/BasicShapes/Cube')))
unreal.log('TDF ITEM DEFINITIONS SAVED AND VALIDATED')
