import unreal
unreal.EditorPythonScripting.set_keep_python_script_alive(True)
levels=unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
assert levels.load_level('/Game/Game/Maps/Test/M_Test_PlayerInteraction')
unreal.EditorLevelLibrary.set_level_viewport_camera_info(unreal.Vector(-1100,0,167),unreal.Rotator(pitch=-2,yaw=5,roll=0))
unreal.EditorLevelLibrary.editor_set_game_view(True)
unreal.get_editor_subsystem(unreal.EditorActorSubsystem).set_selected_level_actors([])
unreal.log('TDF PHASE3 PREVIEW READY')
