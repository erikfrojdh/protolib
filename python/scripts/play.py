
import _protolib as pl
from _protolib import File

import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path
plt.ion()


fpath =  pl.test_data_path()/"eiger/sample_master_2.raw"


f = File(fpath)
image = f.read_frame()
plt.figure()
plt.imshow(image)
plt.clim(0,2e5)
# with open(pl.test_data_path()/"eiger/run_d2_f0_0.raw", 'rb') as f:
#     f.seek(112,1)
#     data = np.fromfile(f, count = 512*256, dtype = np.uint32).reshape(256,512)

# plt.figure()
# plt.imshow(data)