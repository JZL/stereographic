from matplotlib import pyplot
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
from pprint import pprint
import math

def rotateXYZ(coord, R):
    N = [coord[0:3]+[1]]
    print(N)
    N = np.transpose(N)
    N = np.matmul(R, N)
    return [N[0][0], N[1][0], N[2][0]]+coord[3:]

fig = pyplot.figure()
ax = Axes3D(fig)
phi = (1+5**.5)/2
#original coords from wikipedia
coordsO = [
          #[x,    y,    z,  #]
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

centerBottom = map(sum, zip(*([coordsO[2]][0:3]+[coordsO[3]][0:3]+[coordsO[11][0:3]])))
centerBottom = [x/3 for x in centerBottom]
"""
for i in range(len(coordsO)):
    for x in range(3):
        coordsO[i][x]-=centerBottom[x]
"""
#3,4,12 is the bottom. Rotate around axis = 3,4
#normal

ax.scatter(*np.reshape(centerBottom, (-1,1)), color="purple")
ax.scatter([0], [0], [0], color="red")
axis = np.subtract(coordsO[4-1][0:3], coordsO[3-1][0:3])


N = np.subtract(axis, coordsO[12-1][0:3])
N[1] = 0;
print(N)
N = N/np.linalg.norm(N)
axis = axis/np.linalg.norm(axis) #= [0,1,0]
print(axis)


v      = np.cross(N, [1, 0, 0])
c      = np.dot  (N, [1, 0, 0]) #like cosine
print(math.degrees(math.acos(c)))
s      = np.linalg.norm(v)   #like sine
print(math.degrees(math.asin(s)))
s      = np.linalg.norm(v)   #like sine
v_x    = np.asarray([[0,    -v[2],    v[1]], [   v[2], 0,    -v[0]], [   -v[1],    v[0], 0]])
axis_x = np.asarray([[0, -axis[2], axis[1]], [axis[2], 0, -axis[0]], [-axis[1], axis[0], 0]])

#wikipedia equation =
#R = cos*I+sin*u_x+(1-cos)(u*TENSOR*u)

R = c*np.identity(3)+ s*v_x+ (1-c)*np.tensordot([v], [v])
#R = np.identity(3)
#R = np.identity(3)+v_x+np.square(v_x)*1/(1+c)
# http://paulbourke.net/geometry/rotate/
P1 = coordsO[2]
ax.plot([0, N[0]], [0, N[1]], [0,N[2]], "b")
#--------
T          = np.identity(4)
T   [0][3] = -1*P1[0]
T   [1][3] = -P1[1]
T   [2][3] = -P1[2]
print("T")
pprint(T)

T_in       = np.identity(4)
T_in[0][3] = P1[0]
T_in[1][3] = P1[1]
T_in[2][3] = P1[2]

axis = np.subtract(coordsO[3-1][0:3], coordsO[4-1][0:3])
axis = axis/np.linalg.norm(axis) #= [0,1,0]
d = math.sqrt(axis[1]**2 + axis[2]**2)
R_x = np.identity(4)
R_x_in = np.identity(4)
if d!=0:
    R_x   [1][1] = axis[2]/d
    R_x   [1][2] = -axis[1]/d
    R_x   [2][1] = axis[1]/d
    R_x   [2][2] = axis[2]/d

    R_x_in[1][1] = axis[2]/d
    R_x_in[1][2] = axis[1]/d
    R_x_in[2][1] = -axis[1]/d
    R_x_in[2][2] = axis[2]/d
else:
    print("d==0")
print("R_X")
pprint(R_x)

R_y    = np.identity(4)
R_y_in = np.identity(4)

R_y   [0][0] = d
R_y   [0][2] = -axis[0]
R_y   [2][0] = axis[0]
R_y   [2][2] = d

R_y_in[0][0] = d
R_y_in[0][2] = axis[0]
R_y_in[2][0] = -axis[0]
R_y_in[2][2] = d
print("R_Y")
pprint(R_y)

R_z          = np.identity(4)
angledV = np.subtract([coordsO[12-1][0:3]],[coordsO[4-1][0:3]])
angledV[0][1] = 0;
angledV = angledV/np.linalg.norm(angledV)
angledV = np.array(angledV)
print("angledV")
pprint(angledV)

goalV = np.subtract([1,0,0], coordsO[4-1][0:3])
goalV[1] = 0;
goalV = goalV/np.linalg.norm(goalV)
goalV = [[1,0,0]]
goalV = np.array(goalV)
print("goalV")
pprint(goalV)
v = np.cross(angledV[0], goalV[0])
s = np.linalg.norm(v) #like sine
c = np.dot  (angledV[0], goalV[0]) #like cosine
print(s)
print(c)
print("--")
theta = math.asin(s)
print(math.sin(theta))
print(math.cos(theta))
"""
R_z[0][0] = math.cos(theta)
R_z[0][1] = -math.sin(theta)
R_z[1][0] = math.sin(theta)
R_z[1][1] = math.cos(theta)
"""
R_z[0][0] = c
R_z[0][1] = -s
R_z[1][0] = s
R_z[1][1] = c

R = np.matmul(T_in, R_x_in)
R = np.matmul(R, R_y_in)
R = np.matmul(R, R_z)
R = np.matmul(R, R_y)
R = np.matmul(R, R_x)
R = np.matmul(R, T)
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

    
print("Coordinates of icosahedron:")
#pprint([x[0:3] for x in coords])
pprint(coords)
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

for i in range(len(faces)):
    xs = []
    ys = []
    zs = []
    for j in range(len(faces[i])+1):
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
#ax.scatter([0], [0], [0], "b")


xs = [x[0] for x in coords]
ys = [x[1] for x in coords]
zs = [x[2] for x in coords]
ls = [x[3] for x in coords]
for x in range(len(xs)):
    #label = '#%d (%f, %f, %f)' % (ls, x, y, z)
    ax.text(xs[x], ys[x], zs[x], '#%d (%f, %f, %f)' % (ls[x], xs[x], ys[x], zs[x]))
#ax.scatter(xs, ys, zs, linewidth=5)
pyplot.show()

