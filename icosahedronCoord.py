from matplotlib import pyplot
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
from pprint import pprint

def rotateXYZ(coord, R):
    N = [coord[0:3]]
    N = np.transpose(N)
    N = np.matmul(R, N)
    return [N[0][0], N[1][0], N[2][0]]+coord[3:]

fig = pyplot.figure()
ax = Axes3D(fig)
phi = (1+5**.5)/2
#original coords from wikipedia
coordsO = [
           [0,    1,  phi,  1],
           [0,   -1,  phi,  2],
           [0,   -1, -phi,  3],
           [0,    1, -phi,  4],
           [1,  phi,    0,  5],
          [-1,  phi,    0,  6],
          [-1, -phi,    0,  7],
           [1, -phi,    0,  8],
         [phi,    0,    1,  9],
        [-phi,    0,    1, 10],
        [-phi,    0,   -1, 11],
         [phi,    0,   -1, 12]
         ]

#3,4,12 is the bottom. Rotate around axis = 3,4
#normal
N = np.cross(
        np.subtract(coordsO[3-1][0:3], coordsO[ 4-1][0:3]),
        np.subtract(coordsO[3-1][0:3], coordsO[12-1][0:3]))
N = N/np.linalg.norm(N)



v = np.cross(N, [0,0,-1])
s = np.linalg.norm(v)
c = np.dot(N, [0,0,-1])
v_x = [[0, -v[2],v[1]],[v[2], 0, -v[0]],[-v[1], v[0], 0]]
R = np.identity(3)+v_x+np.square(v_x)*1/(1+c)
"""
print("N original")
pprint(N)
N = np.transpose([N])
print("N^T")
pprint(N)
N = np.matmul(R, N)
print("Rotated N")
pprint(N)
pprint(np.transpose(N)[0])
"""
coords = [rotateXYZ(x, R) for x in coordsO]

centerBottom = map(sum, zip(*([coords[2]][0:3]+[coords[3]][0:3]+[coords[11][0:3]])))
centerBottom = [x/3 for x in centerBottom]
for i in range(0,len(coords)):
    for x in range(0,3):
        coords[i][x]-=centerBottom[x]
    
print("Coordinates of icosahedron:")
pprint([x[0:3] for x in coords])
"""
 To see if unique, copy this, paste in terminal, then copy the indicies
 pp | sed "s/[^0-9,]//g"|while read line; do echo $line | sed 's/,/\n/g' | sort | awk '{line=line" "$0} END {print line}' ; done|sort|uniq -d
"""
#ga left arrow, enter *, w/ easy align
faces = [
          [3,  4, 12],
          [3,  4, 11],
          [3, 12,  8],
          [8,  9, 12],
         [12,  9,  5],
          [9,  5,  1],
         [12,  4,  5],
          [3,  7,  8],
          [7,  8,  2],
         [10,  2,  7],
          [2,  8,  9],
         [11,  3,  7],
         [10, 11,  6],
          [4, 11,  6],
          [4,  6,  5],
          [5,  6,  1],
          [6, 10,  1],
          [1,  2,  9],
          [1, 10,  2],
         [10,  7, 11]
        ]

for i in range(0, len(faces)):
    xs = []
    ys = []
    zs = []
    for j in range(0, len(faces[i])+1):
        x = coords[faces[i][(j%len(faces[i]))]-1][0]
        y = coords[faces[i][(j%len(faces[i]))]-1][1]
        z = coords[faces[i][(j%len(faces[i]))]-1][2]

        l = coords[faces[i][(j%len(faces[i]))]-1][3]

        xs.append(x)
        ys.append(y)
        zs.append(z)
        label = '#%d' % (l)
        #ax.text(x, y, z, label)
    ax.plot(xs, ys, zs, "g-",alpha=0.5)
ax.scatter([0], [0], [0], "b")
ax.plot([0, N[0]], [0, N[1]], [0,N[2]], "b")

xs = [x[0] for x in coords]
ys = [x[1] for x in coords]
zs = [x[2] for x in coords]
ls = [x[3] for x in coords]
for x in range(0, len(xs)):
    #label = '#%d (%f, %f, %f)' % (ls, x, y, z)
    ax.text(xs[x], ys[x], zs[x], ls[x])
#ax.scatter(xs, ys, zs, linewidth=5)
ax.scatter(*np.reshape(centerBottom, (-1,1)))
pyplot.show()

