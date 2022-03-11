from protolib import File, test_data_path

#plotting and data access
import numpy as np
import matplotlib.pyplot as plt
plt.ion()




fpath =  test_data_path()/"jungfrau/run_master_0.raw"

with File(fpath) as f:
    print("before")
    data = f.read()
    print("after")

fig, ax = plt.subplots(2,1, figsize = (9,10))
im = ax[0].imshow(data[10])
ax[1].plot(data[:,50,50])

# The f.read() reads all frames from the file into a numpy array 
# r.read(N) reads N files and it is also possible to use the C++
# read_frame

# f = File(fpath)
# image = f.read_frame()