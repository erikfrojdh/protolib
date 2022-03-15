


import numpy as np

magic_str = b'\x93NUMPY'


def parse(fname):
    print(fname)
    with open(fname, 'rb') as f:
        s = f.read(6)
        print(s)
        major_ver = f.read(1)
        minor_ver = f.read(1)
        print(f'{major_ver=}')
        print(f'{minor_ver=}')
        header_size = f.read(2)
        header_size_int = int.from_bytes(header_size, 'little')
        print(f'{header_size=}: {header_size_int}')
        header = f.read(header_size_int)
        print(f"\"{header}\"")
        



parse("test.npy")
parse("test2.npy")
# f1 = open("test.npy", "rb")
# s = f1.read(6)
# assert s == magic_str
# major_ver = f1.read(1)
# minor_ver = f1.read(1)

# f2 = open("test2.npy", "rb")
# s = f2.read(6)
# assert s == magic_str