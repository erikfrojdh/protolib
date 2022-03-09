

import _protolib as pl
from _protolib import File

import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path
plt.ion()

path = Path("/Users/erik/data/clara/000_01-Mar-2022_110029/")
fpath =  path/"run_master_0.raw"


with File(fpath) as f:
    data = f.read()


fig, ax = plt.subplots(2,1)
im = ax[0].imshow(data[10])
ax[1].plot(data[:,50,50])

