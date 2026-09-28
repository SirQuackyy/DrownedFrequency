import unreal
unreal.EditorPythonScripting.set_keep_python_script_alive(True)
levels=unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
assert levels.load_level('/Game/Game/Maps/Test/M_Test_SaveStreaming')
unreal.EditorLevelLibrary.set_level_viewport_camera_info(unreal.Vector(200,0,165),unreal.Rotator(pitch=-3,yaw=10,roll=0))
unreal.EditorLevelLibrary.editor_set_game_view(True)
unreal.get_editor_subsystem(unreal.EditorActorSubsystem).set_selected_level_actors([])
unreal.log('TDF PHASE6 PREVIEW READY - Play streams Archive 01 and starts objectives')
