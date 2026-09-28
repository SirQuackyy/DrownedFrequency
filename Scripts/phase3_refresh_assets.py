import runpy
from pathlib import Path
base=Path(__file__).parent
for name in ['phase3_door_assets.py','phase3_switch_assets.py','phase3_item_assets.py',
             'phase3_world_item_assets.py','phase3_inspection_assets.py','phase3_atmosphere.py']:
    runpy.run_path(str(base/name))
runpy.run_path(str(base/'phase3_test_map.py'))
