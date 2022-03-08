

import _protolib as pl
from pathlib import Path

import matplotlib.pyplot as plt
plt.ion()

path = Path("/home/l_frojdh/software/hdf5maker/tests/data/run_master_16.raw")
f = pl.File(path)
img = f.read_frame()
fig,ax = plt.subplots()
im = ax.imshow(img)
im.set_clim(0,8000)


