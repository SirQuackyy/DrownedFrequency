import unreal
unreal.EditorPythonScripting.set_keep_python_script_alive(True)

levels = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
assert levels.load_level('/Game/Game/Maps/Persistent/L_TDF_Persistent')
unreal.EditorLevelLibrary.set_level_viewport_camera_info(unreal.Vector(-650, -580, 167), unreal.Rotator(pitch=-3, yaw=32, roll=0))
unreal.EditorLevelLibrary.editor_set_game_view(True)
unreal.get_editor_subsystem(unreal.EditorActorSubsystem).set_selected_level_actors([])
unreal.log('TDF PREVIEW READY: Persistent foundation room. Press Play to test the player.')
