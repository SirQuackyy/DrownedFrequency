import runpy
from pathlib import Path
base=Path(__file__).parent
for script in ['phase4_templates.py','phase4_input_ui.py','phase4_visibility.py','phase4_test_map.py']:
    runpy.run_path(str(base/script),run_name='__main__')
