
import numpy as np
import matplotlib.pyplot as plt
plt.ion()


import protolib as pl


fpath =  pl.test_data_path()/"eiger/sample_master_2.raw"

f = pl.File(fpath)
img = f.read_frame()

fig, ax = plt.subplots()
im = ax.imshow(img)
im.set_clim(0,1e5)