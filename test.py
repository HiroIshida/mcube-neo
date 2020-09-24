import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
from mpl_toolkits.mplot3d import Axes3D
from skimage import measure
import build.mcube as mcube

N = 50
w = 1.2
x = y = z = np.linspace(-w, w, N) 
X, Y, Z = np.meshgrid(x, y, z)
pts = np.array(list(zip(X.flatten(), Y.flatten(), Z.flatten())))
def func(X):
    r_lst = np.sum(X ** 2, axis=1)
    return np.cos(r_lst * 3.14)

F = func(pts)
shape = [N]*3
isovalue = 0.0
from time import time
ts = time()
for i in range(100):
    mcube.marching_cube(F, shape, isovalue)
print(time() - ts)
