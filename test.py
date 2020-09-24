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
for i in range(10):
    V_, P_ = mcube.marching_cube(F, shape, isovalue)
    V = np.array(V_).reshape(len(V_)//3, 3)
    P = np.array(P_).reshape(len(P_)//3, 3)
print(time() - ts)

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.scatter(V[:, 0], V[:, 1], V[:, 2])
plt.show()