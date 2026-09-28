from pathlib import Path
for name in ['phase5_caption_ui.py','phase5_templates.py','phase5_test_map.py']:
    exec(compile((Path(__file__).parent/name).read_text(encoding='utf-8-sig'),name,'exec'))
