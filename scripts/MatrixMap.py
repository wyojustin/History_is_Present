import numpy as np
import pylab as pl

def fmt(v):
    return '%3d' % v
def toc(arr):
    print("#idndef MATRIX_MAP_H")
    print("#define MATRIX_MAP_H")
    print("#define MatrixWidth 16")
    print("#define MatrixHeight 24")

    print('MatrixMap[24][16] = {')
    for l in arr:
        print('    {' + ','.join(map(fmt, l)) + '},')
    print('    };')
    print("#endif")
clockiot = np.array([[  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15], 
                     [ 31,  30,  29,  28,  27,  26,  25,  24,  23,  22,  21,  20,  19,  18,  17,  16], 
                     [ 32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47], 
                     [ 63,  62,  61,  60,  59,  58,  57,  56,  55,  54,  53,  52,  51,  50,  49,  48], 
                     [ 64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79], 
                     [ 95,  94,  93,  92,  91,  90,  89,  88,  87,  86,  85,  84,  83,  82,  81,  80], 
                     [ 96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111], 
                     [127, 126, 125, 124, 123, 122, 121, 120, 119, 118, 117, 116, 115, 114, 113, 112]])
hip = np.vstack([clockiot + 0 * 123, clockiot + 128, clockiot + 2 * 128])
print(toc(hip))

