

import os
def find_folder(path, start):
    if isinstance(start, int):
        start = f'{start:03}'
    folder = [f for f in os.listdir(path) if f.startswith(start + "_")]
    if len(folder) == 1:
        return path / folder[0]
    else:
        raise ValueError("Could not find measurement directory")