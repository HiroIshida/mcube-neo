import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
from mpl_toolkits.mplot3d import Axes3D
import mcube as mcube

N = 30
w = 1.2
x = y = z = np.linspace(-w, w, N) 
X, Y, Z = np.meshgrid(x, y, z)
pts = np.array(list(zip(X.flatten(), Y.flatten(), Z.flatten())))
def func(X):
    r_lst = np.sum(X ** 2, axis=1)
    return np.cos(r_lst * 3.14)

Z = func(pts)
shape = [N]*3
isovalue = 0.0
V, F, Cv, Cf = mcube.marching_cube(Z, shape, isovalue)
n_group = np.max(Cv) + 1

import matplotlib.cm as cm
cmap = cm.get_cmap(name='rainbow')

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
colors = [cmap(25*i) for i in range(n_group)]

useCv = False
if useCv:
    for color, idx_color in zip(colors, range(n_group)):
        idx_verts = np.array(Cv) == idx_color
        ax.scatter(V[idx_verts, 0], V[idx_verts, 1], V[idx_verts, 2], c=color)
else:
    for color, idx_color in zip(colors, range(n_group)):
        idx_verts = list(set(F[np.array(Cf) == idx_color].flatten()))
        ax.scatter(V[idx_verts, 0], V[idx_verts, 1], V[idx_verts, 2], c=color)
plt.tight_layout()
plt.show()
