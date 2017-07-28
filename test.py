from matplotlib import pyplot
from mpl_toolkits.mplot3d import Axes3D
import numpy as np

fig = pyplot.figure()
ax = Axes3D(fig)
ax.set_xlim(490, 510)

x,y,z = np.loadtxt("out/out.pgm", delimiter=',', unpack=True)
ax.scatter(x,y,z)
pyplot.show()
