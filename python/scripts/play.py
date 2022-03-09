
import _protolib as pl
from _protolib import File

import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path
plt.ion()


fpath =  pl.test_data_path()/"jungfrau/run_master_0.raw"


f = File(fpath)
arr = f.test_read()
plt.imshow(arr)