

import _protolib as pl

import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path
plt.ion()

path = Path("/Users/erik/data/clara/000_01-Mar-2022_110029/")
fpath =  path/"run_master_0.raw"


with pl.File(fpath) as f:
    print(f'Total frames: {f.total_frames()}')
    img = f.read_frame()
    data = f.read()


# fig, ax = plt.subplots()
# im = ax.imshow(img)