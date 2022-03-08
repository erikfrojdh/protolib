

import _protolib as pl

import numpy as np
import matplotlib.pyplot as plt
plt.ion()


fname =  "/Users/erik/data/clara/000_01-Mar-2022_110029/run_master_0.raw"
f = pl.File(fname)
img = f.read_frame()
fig, ax = plt.subplots()
im = ax.imshow(img)