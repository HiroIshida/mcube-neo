import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
from mpl_toolkits.mplot3d import Axes3D
import build.mcube as mcube

N = 15
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
for i in range(1):
    V, E, NF = mcube.marching_cube(F, shape, isovalue)
print(time() - ts)

def dfs(E, N):
    isVisited = np.array([False]*len(V))

    def recursion(idx_here, isVisited, group):
        isVisited[idx_here] = True
        group.append(idx_here)

        faces_near = N[idx_here]
        for idx_face in faces_near:
            face = E[idx_face]
            for idx_vert in face:
                if not isVisited[idx_vert]:
                    print("hoge")
                    recursion(idx_vert, isVisited, group)

    def make_group(idx_init):
        group = []
        recursion(idx_init, isVisited, group)
        return group

    group_list = []
    while(True):
        print("hoge")
        pair = next(((x, idx) for (x, idx) in \
                zip(list(isVisited), range(len(isVisited))) if ~x), None)
        if pair is None:
            break
        group_list.append(make_group(pair[1]))

    return isVisited, group_list

ts = time()
isVisited, group_list = dfs(E, NF)
print(time() - ts)
import matplotlib.cm as cm
cmap = cm.get_cmap(name='rainbow')

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
colors = [cmap(40*i) for i in range(len(group_list))]
for group, color in zip(group_list, colors):
    ax.scatter(V[group, 0], V[group, 1], V[group, 2], c=color)
plt.show()
