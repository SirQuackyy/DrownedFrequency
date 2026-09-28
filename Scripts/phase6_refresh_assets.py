from pathlib import Path
for name in ['phase6_base_maps.py','phase6_checkpoint_assets.py','phase6_progression_assets.py','phase6_save_ui.py','phase6_lighting_pass.py']:
    exec(compile((Path(__file__).parent/name).read_text(encoding='utf-8-sig'),name,'exec'))
