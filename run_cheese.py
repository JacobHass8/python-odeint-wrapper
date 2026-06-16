import sys 
sys.path.append("./builddir")
import cheese
import numpy as np
from matplotlib import pyplot as plt
from scipy.integrate import solve_ivp
import time

dt = 0.1
t0 = 0
tn = 10
q0 = np.array([2])
p0 = np.array([-1])

def q2dp(vals):
    return -np.array(vals)

def p2dq(vals):
    return np.array(vals)


s = time.time()
data = cheese.py_solve(dt, t0, tn, q0, p0, q2dp, p2dq)
print(time.time() - s)
def dx(t, x):
    return np.array([x[1], -x[0]])

x0 = np.array([2, -1])
t_span = (t0, tn)
t_eval = np.arange(t_span[0], t_span[1], dt)

s = time.time()
solution = solve_ivp(dx, t_span, x0, t_eval=t_eval, method='RK45')
print(time.time() - s)
t = solution.t
x = solution.y

p = np.array(data.p)
q = np.array(data.q)

fig, ax = plt.subplots()
ax.plot(p, q, c='r')
ax.plot(x[0], x[1], c='b', ls='--')
fig.savefig("Plot.png", bbox_inches='tight')

r = np.sqrt(5)
fig, ax = plt.subplots()
ax.plot(np.sqrt(p**2 + q**2) - r, c='r')
ax.plot(np.sqrt(x[0]**2 + x[1]**2)-r, c='b')
fig.savefig("Residual.png")