import numpy as np
import matplotlib.pyplot as plt
from ast import literal_eval

def _reverse(data):
    return [data[i] for i in range(len(data)-1, -1, -1)]

def poly(x, *args):
    pol = 0
    for i in range(len(args)):
        pol += args[i] * np.power(x, i)
    return pol

def _mean(data):
    return sum(data) / len(data)

def detrend(data, order = 2, window = 1):
    if window == 0:
        return _detrend(data, 1, 1)
    l = len(data)
    pos = [i for i in range(l) if i < window or l - 1 - i < window]
    data_l = [data[i] for i in pos]
    fit_l = _reverse(np.polyfit(pos, data_l, order))
    data_l = [poly(i, *fit_l) for i in range(l)]
    mean_l = _mean(data_l)
    data = [data[i] - data_l[i] for i in range(l)]
    return data


path = "gpx2_12.dat"

# chunk = 64
# f = open(path, 'rb')
# row = 0
# row = f.read(chunk)
# data = []
# while row:
#     data.append(row)
#     row = f.read(chunk)
# f.close()

y = np.fromfile("gpx2_11.dat", dtype = np.double)
#yn = np.mod(y, 100000e-20)
yn = detrend(y, 1)
x = list(range(1, len(y) + 1))

plt.subplot(311)
plt.plot(x, y)
plt.ylabel("Value")
plt.xlabel("Sample Count")
plt.title("")

plt.subplot(312)
plt.plot(x, yn)
plt.ylabel("Value")
plt.xlabel("Sample Count")
plt.title("")

plt.subplot(313)
plt.hist(yn)
plt.xlabel("Value")
plt.ylabel("Frequency")

plt.tight_layout()
plt.show(block=1)
