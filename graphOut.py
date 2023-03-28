import matplotlib.pyplot
import sys
import math

values = []
x = [(i+1) / 100 for i in range(100)]

for i in sys.argv[1:]:
    values.append((float(i)))

plot = matplotlib.pyplot

plot.scatter(values, x, facecolors='black',alpha=0.55, s=10)
plot.plot(values, x, alpha=0.1)
plot.yticks([0, 0.05, 0.25, 0.50, 0.75, 0.95, 1])
plot.xticks()

plot.annotate("5%: " + str((values[4])) + " seconds\n25%: " + str(values[24]) + " seconds\n50%: " + str((values[49])) + " seconds\n75%: " + str((values[74])) + " seconds\n95%: " + str((values[94])) + " seconds", [values[99] * 2/3, 0.5])

plot.ylabel("Percentage of behaviours executed")
plot.xlabel("Behaviour Latency (Seconds)")

plot.scatter(values[4], 0.05, color="red")
plot.plot([values[4] for i in range(100)], [0.04 for i in range(100)], alpha=0.3)

plot.scatter(values[24], 0.25, color="red")
plot.plot([values[24] for i in range(100)], [0.24 for i in range(100)], alpha=0.3)


plot.scatter(values[49], 0.50, color="red")
plot.plot([values[49] for i in range(100)], [0.49 for i in range(100)], alpha=0.3)

plot.scatter(values[74], 0.75, color="red")
plot.plot([values[74] for i in range(100)], [0.74 for i in range(100)], alpha=0.3)

plot.scatter(values[94], 0.95, color="red")
plot.plot([values[94] for i in range(100)], [0.94 for i in range(100)], alpha=0.3)



plot.show()