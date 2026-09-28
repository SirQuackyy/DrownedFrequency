"""Read-only audit of authored persistent identity and sector references."""
import unreal
lib=unreal.EditorAssetLibrary
levels=unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
actors=unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
seen={}
count=0
maps=[p.split('.')[0] for p in lib.list_assets('/Game/Game/Maps',True,False) if str(lib.find_asset_data(p).asset_class_path.asset_name)=='World']
for path in sorted(maps):
    assert levels.load_level(path),path
    for a in actors.get_all_level_actors():
        c=a.get_component_by_class(unreal.TDFStableActorComponent)
        if c is None or not c.get_editor_property('persist_in_save'): continue
        guid=c.get_editor_property('stable_actor_id')
        identity=guid.to_string()
        assert unreal.GuidLibrary.is_valid_guid(guid),(path,a.get_name(),'invalid GUID')
        actorpath=a.get_path_name()
        assert identity not in seen or seen[identity]==actorpath,(identity,seen.get(identity),actorpath)
        seen[identity]=actorpath
        count+=1
sectorids={}
for path in lib.list_assets('/Game/Game/Data/Sectors',True,False):
    a=lib.load_asset(path)
    if not isinstance(a,unreal.TDFSectorDataAsset): continue
    identity=str(a.get_editor_property('sector_id'))
    assert identity not in sectorids,(identity,path)
    assert lib.does_asset_exist(str(a.get_editor_property('level_package_name'))),path
    sectorids[identity]=a
for identity,a in sectorids.items():
    for neighbor in a.get_editor_property('neighbor_sector_ids'): assert str(neighbor) in sectorids,(identity,neighbor)
unreal.log('TDF FINAL AUDIT PASSED: %d maps, %d distinct persistent actors, %d sectors'%(len(maps),len(seen),len(sectorids)))
