

import protolib as pl
import time
import numpy as np
import matplotlib.pyplot as plt
from pathlib import Path
import boost_histogram as bh
plt.ion()

import seaborn as sns
from protolib.PythonClusterFinder import PythonClusterFinder


# # path = pl.test_data_path()/"jungfrau"

# # image = np.load(path/'sparse.npy')
# # # image = image[337:353,168:192]

# # # image = np.load(path/'sparse.npy')[175:350, 250:450]
# # # image = image[120:140, 30:50]
# # threshold = 5
# # cf = pl.ClusterFinder(image.shape, threshold)

# # cf.find_clusters(image)
# # hits = cf.hits()

# # pycf = PythonClusterFinder(threshold)
# # pycf.find_clusters(image)
# # hits2 = pycf.clusters()

# # assert hits.size == hits2.size, "size error"
# # assert hits['energy'].mean() == hits2['energy'].mean(), "mean energy error"

# # arr = hits2['energy']/hits2['size']
# # suspects =[]
# # for i,a in enumerate(arr):
# #     if a%1:
# #         print(f"{i}:{a}")
# #         suspects.append(i)
# #         suspects.append(int(a))

# # for label in suspects:
# #     rows,cols = np.where(label_img==label)
# #     for r,c in zip(rows, cols):
# #         print(f'{r},{c} {label_img[r,c]}')

# # # plt.figure()
# # # plt.imshow(label_img2)


# # binary1 = np.zeros(image.shape)
# # binary2 = np.zeros(image.shape)

# # binary1[label_img>0] = 1
# # binary2[label_img2>0] = 1

# # plt.figure()
# # sns.heatmap(label_img, annot=True, fmt="d")

# # plt.figure()
# # sns.heatmap(label_img2, annot=True, fmt="d")

path = Path('/Users/erik/data/clara/061_04-Mar-2022_121457')
cal = np.load('/Users/erik/software/clara/data/calibration.npy')
pd = np.load('/Users/erik/data/clara/054_04-Mar-2022_102516/pedestal.npy')

n_images = 100
# error = False
# pycf = PythonClusterFinder(5)
# cf = pl.ClusterFinder((512,1024), 5)
# with pl.File(path/'run_master_0.raw') as f:
#     for i in range(n_images):
#         raw_image = f.read_frame()
#         data = pl.apply_calibration(raw_image, pd, cal)
#         cf.find_clusters(data)
#         pycf.find_clusters(data)
#         if(cf.total_clusters() != pycf.total_clusters()):
#             print(f'Stopping at frame: {i}')
#             print(f'C++: {cf.total_clusters()}')
#             print(f'Python: {pycf.total_clusters()}')
#             error = True
#             break


# print(f'C++: {cf.total_clusters()}')
# print(f'Python: {pycf.total_clusters()}')

# hits = cf.hits()
# hits2 = pycf.hits()

# # if error:

# pycf2 = PythonClusterFinder(0.5, check_equal = True)
# cpp_labels = cf.labeled()
# p =pycf2.find_clusters(cpp_labels)


# if not isinstance(p, np.ndarray):
#     bbox = p.bbox
#     data_roi = data[bbox[0]-2:bbox[2]+2, bbox[1]-2:bbox[3]+1]
#     cf2 = pl.ClusterFinder(data_roi.shape, 5)
#     cf2.single_pass(data_roi)
#     labels = cf2.labeled()
#     sns.heatmap(labels, annot=True, fmt="d")
#     cf2.print_connections()
# # else:
# #     cf2 = pl.ClusterFinder(data.shape, 0.5)
# #     cf2.find_clusters(py_labels)
# #     cpp_labels = cf2.labeled()

# py_hits = pycf2.hits()

# arr = py_hits['energy']/py_hits['size']
# s = np.sort(arr, axis=None)
# pos = np.where((np.diff(s)==0))
# val = s[pos[0]]

# pos = np.where(arr==val)

# cf2 = pl.ClusterFinder(data.shape, 5)
# cf2.single_pass(data)
# labels = cf2.labeled()


# image = np.zeros((10,10))
# image[5,9] = 1
# image[6,9] = 1
# image[6,0] = 1
# cf2 = pl.ClusterFinder(image.shape, .5)
# cf2.find_clusters(image)
# sns.heatmap(cf2.labeled(), annot=True, fmt="d")


# i = 0
t0 = time.perf_counter()
cf = pl.ClusterFinder((512,1024), 5)

with pl.File(path/'run_master_0.raw') as f:
    for i in range(n_images):
        raw_image = f.read_frame()
        data = pl.apply_calibration(raw_image, pd, cal)
        cf.find_clusters(data)
print(f"C++ clustering: {time.perf_counter()-t0:.3}s")
hits = cf.hits()
print(f"found: {hits.size} clusters")


pcf = PythonClusterFinder(5)
with pl.File(path/'run_master_0.raw') as f:
    for i in range(n_images):
        raw_image = f.read_frame()
        data = pl.apply_calibration(raw_image, pd, cal)
        pcf.find_clusters(data)
print(f"Python clustering: {time.perf_counter()-t0:.3}s")
xmin = 0
xmax = 300


hits2 = pcf.hits()
t0 = time.perf_counter()
hist = bh.Histogram(bh.axis.Regular(bins=100, start=xmin, stop=xmax))
hist.fill(hits['energy'])
fig, ax = plt.subplots(1,2, figsize = (10,6))
ax[0].bar(hist.axes[0].centers, hist.view(), width=hist.axes[0].widths, label = 'Measured')

peak = hist.view().max()
with np.load('/Users/erik/software/clara/data/edep.npz') as f:
    x = f['x']
    y = f['y']
ax[0].plot(x, y*(peak/y.max()), '-', color = 'red', lw = 2, label = 'GEANT4')
ax[0].set_xlabel('Edep/e- [keV]')
ax[0].legend()
ax[0].set_xlim(xmin, xmax)

hist2 = bh.Histogram(bh.axis.Regular(bins=10, start=0.5, stop=10.5))
hist2.fill(hits['size'])
ax[1].bar(hist2.axes[0].centers, hist2.view(), width=hist2.axes[0].widths, label = 'Measured')
ax[1].set_xlabel("Cluster size")
ax[1].legend()
ax[1].set_xlim(0.5, 10.5)
print(f"fill and plot histograms: {time.perf_counter()-t0:.3}s")
fig.suptitle("C++", size = 22)






hist = bh.Histogram(bh.axis.Regular(bins=100, start=xmin, stop=xmax))
hist.fill(hits2['energy'])
fig, ax = plt.subplots(1,2, figsize = (10,6))
ax[0].bar(hist.axes[0].centers, hist.view(), width=hist.axes[0].widths, label = 'Measured')

peak = hist.view().max()
with np.load('/Users/erik/software/clara/data/edep.npz') as f:
    x = f['x']
    y = f['y']
ax[0].plot(x, y*(peak/y.max()), '-', color = 'red', lw = 2, label = 'GEANT4')
ax[0].set_xlabel('Edep/e- [keV]')
ax[0].legend()
ax[0].set_xlim(xmin, xmax)

hist2 = bh.Histogram(bh.axis.Regular(bins=10, start=0.5, stop=10.5))
hist2.fill(hits2['size'])
ax[1].bar(hist2.axes[0].centers, hist2.view(), width=hist2.axes[0].widths, label = 'Measured')
ax[1].set_xlabel("Cluster size")
ax[1].legend()
ax[1].set_xlim(0.5, 10.5)
fig.suptitle("Python", size = 22)