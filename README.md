# protolib - No the name is not fixed

Support library for analyzing data from PSI pixel and strip detectors

### Aim

* Make analyzing data from our detectors easier
* Quicker to get started for new members
* Work well with ROOT...
* ...and as well with Python
* Usable as a c++ library
* Value semantics
* Limited (no?) manual memory management

### Dependencies

* libfmt
* pybind11 > 2.7.0
* C++17? (module load gcc on RH7)
* Optional HDF5 (for our files)
* TBB? 
* Optional ROOT? or make it an integral part? 

### Work in progress

* algorithms that operate on files and in memory
* write numpy files


# Examples:

## Iterate over images on disk

For python we use numpy arrays in C++ we might need our own image data container.

```cpp
#include <protolib/File.hpp>
pl::File data_file("run_master_0.raw")
for(auto& image : data_file)
    //do something

```


```python
from protolib import File
with File("run_master_0.raw") as data_file:
    for image in data_file:
        #do something

```

## Load everything into memory

<!-- ```cpp
auto data = pl::open_file("run_master_0.raw").read()
for (auto& image : data)
    //do something
``` -->

```python
with File("run_master_0.raw") as f:
    data = f.read()
```

## Access pixels

```cpp
//Runtime depending on type of file
File f("run_master_0.raw");
auto image = f.read_frame();
uint16_t pixel = image(120, 140);


//compile time
auto f = pl::MythenRawFile<32>("run_master_0.raw");
MytenRawImage<32> image = f.read(1);
```