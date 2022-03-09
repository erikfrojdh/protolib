"""
The code works more or less like the C++ example with
the exception that we can dynamically create a numpy
array of the right type

"""

from protolib import File, test_data_path

fpath = test_data_path()/"jungfrau/run_master_0.raw"
f = File(fpath)

total = 0
for frame in f:
    total += frame[100,100]

print(f"The sum of pixel 100,100 is {total}\n")

