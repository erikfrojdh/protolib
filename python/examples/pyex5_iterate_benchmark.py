"""
The code works more or less like the C++ example with
the exception that we can dynamically create a numpy
array of the right type

"""

from protolib import File, test_data_path
import protolib as pl
import time
import numpy as np
fpath = test_data_path()/"jungfrau/run_master_0.raw"
direct_path = test_data_path()/"jungfrau/run_d0_f0_0.raw"

def iterate_using_library():
    t0 = time.perf_counter()
    f = File(fpath)
    total = 0
    for frame in f:
        total += frame[100,100]
    return time.perf_counter()-t0

def cpp_sum():
    t0 = time.perf_counter()
    pl.sum_pixel(fpath)
    return time.perf_counter()-t0

def cpp_direct():
    t0 = time.perf_counter()
    pl.sum_pixel_direct(direct_path)
    return time.perf_counter()-t0

def cpp_custom():
    t0 = time.perf_counter()
    pl.clean_read(direct_path)
    return time.perf_counter()-t0

def full_read():
    t0 = time.perf_counter()
    with File(fpath) as f:
        data = f.read()
    # total = data[:,100,100].sum()
    return time.perf_counter()-t0

def iterate_using_numpy():
    t0 = time.perf_counter()
    total = 0
    with open(test_data_path()/"jungfrau/run_d0_f0_0.raw", 'rb') as f:
        for i in range(100):
            f.seek(112, 1)
            frame = np.fromfile(f, count = 512*1024, dtype=np.uint16).reshape(512,1024)
            total += frame[100,100]
    return time.perf_counter()-t0

def call_overhead():
    t0 = time.perf_counter()
    total = 0
    total += pl.call_overhead()
    return time.perf_counter()-t0


print(pl.clean_read(direct_path))
N = 50
t_library = 0
t_numpy = 0
t_call = 0
t_cpp = 0
t_cpp_direct = 0
t_cpp_custom = 0
t_full_read = 0
for i in range(N):
    t_numpy += iterate_using_numpy()
    t_library += iterate_using_library()
    # t_call += call_overhead()
    t_cpp += cpp_sum()
    t_cpp_direct += cpp_direct()
    t_cpp_custom += cpp_custom()
    t_full_read += full_read()

print(f"Library: {t_library:.3}s")
print(f"Numpy: {t_numpy:.3}s")
# print(f"call: {t_call:.3}s")
print(f"cpp: {t_cpp:.3}s")
print(f"cpp direct: {t_cpp_direct:.3}s")
print(f"cpp custom: {t_cpp_custom:.3}s")
print(f"full_read: {t_full_read:.3}s")