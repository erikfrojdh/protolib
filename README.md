# protolib

Support library for analyzing data from PSI pixel and strip detectors

### Aim

* Make analyzing data from our detectors easier
* Quicker to get started for new memebers
* Work well with ROOT...
* ...and as well with Python
* Usable as a c++ library

### Dependencies

* C++17? (module load gcc on RH7)
* Maybe but probably not: https://xtensor.readthedocs.io/en/latest/index.html
* Optional HDF5 (for our files)


# Examples:

## Iterate over images on disk

For python we use numpy arrays in C++ we might need our own image data container.

```cpp
namespace pl = protolib; 
auto file = pl::open_file("run_master_0.raw")
for (auto image : file)
    //do something

```


```python
from protolib import open_file
with open_file("run_master_0.raw") as f:
    for image in f:
        #do something

```

## Load everything into memory

```cpp
auto data = pl::open_file("run_master_0.raw").read()
for (auto& image : data)
    //do something
```

```python
with open_file("run_master_0.raw") as f:
    data = f.read()
```

## Access pixels

```cpp
//Runtime depending on type of file
auto f = pl::open_file("run_master_0.raw");
auto image = f.read(1);

uint16_t pixel = image(120, 140);


//compile time
auto f = pl::MythenRawFile<32>("run_master_0.raw");
MytenRawImage<32> image = f.read(1);
```