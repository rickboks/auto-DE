import matplotlib.pyplot as plt 
import matplotlib.animation as animation 
from mpl_toolkits.mplot3d import Axes3D
import sys

Writer = animation.writers['ffmpeg']
writer = Writer(fps=8)

fig = plt.figure(figsize=(8,8)) 
ax = fig.add_subplot(1,1,1, projection='3d')
ax.set_xlim3d([-5, 5])
ax.set_xlabel('X')
ax.set_ylim3d([-5, 5])
ax.set_ylabel('Y')
ax.set_zlim3d([-5, 5])
ax.set_zlabel('Z')

plt.subplots_adjust(left=0, bottom=0, right=1, top=1, wspace=0, hspace=0)

sc = ax.scatter([],[],[], s=20)

def init(): 
	return sc

# animation function 
def animate(frame): 
    angle = frame['i'] % 360
    xdata, ydata, zdata = frame['x'], frame['y'], frame['z']
    ax.view_init(30, int(angle))
    sc._offsets3d = (xdata, ydata, zdata)
    return sc 

frames = []
i = 0

while True:
    inp = sys.stdin.readline().rstrip('\n')
    if not inp:
        break

    frame = {'i' : i,  'x': [], 'y' : [], 'z' : []}
    while inp != "":
        x,y,z = [ float(x) for x in inp.split() ][:3]
        frame['x'].append(x)
        frame['y'].append(y)
        frame['z'].append(z)
        inp = sys.stdin.readline().rstrip('\n')

    frames.append(frame)
    i+=1

# plt.axis('off') 
anim = animation.FuncAnimation(fig, animate, init_func=init, interval=5, frames=frames) 
anim.save(sys.argv[1], writer=writer, dpi=200)
# plt.show()
