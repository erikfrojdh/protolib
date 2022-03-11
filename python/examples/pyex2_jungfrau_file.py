

from protolib import JungfrauRawFile, test_data_path
import matplotlib.pyplot as plt
plt.ion()

fpath = test_data_path()/"jungfrau/run_d0_f0_0.raw"
f = JungfrauRawFile(fpath, 512, 1024)
data = f.read()


plt.imshow(data[99])

print(f"{data[:,100,100].sum()=}")