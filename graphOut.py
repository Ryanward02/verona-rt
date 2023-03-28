import matplotlib.pyplot
import sys

values = []
x = [(i+1) / 100 for i in range(100)]

for i in sys.argv[1:]:
    values.append(float(i))

plot = matplotlib.pyplot

plot.scatter(values, x, facecolors='black',alpha=0.55, s=10)

plot.title("")
plot.xlabel("Percentage of behaviours executed (%)")
plot.ylabel("Behaviour Latency (Seconds)")

plot.scatter(5, values[4], color="red")
plot.annotate("5%: " + str(int(values[4])), (10, values[4]))
plot.plot([(i + 1) / 100 for i in range(100)], [values[4] for i in range(100)], alpha=0.3)

plot.scatter(25, values[24], color="red")
plot.annotate("25%: " + str(int(values[24])), (0, values[24]))
plot.plot([(i + 1) / 100 for i in range(100)], [values[24] for i in range(100)], alpha=0.3)


plot.scatter(50, values[49], color="red")
plot.annotate("50%: " + str(int(values[49])), (0, values[49]))
plot.plot([(i + 1) / 100 for i in range(100)], [values[49] for i in range(100)], alpha=0.3)

plot.scatter(75, values[74], color="red")
plot.annotate("75%: " + str(int(values[74])), (0, values[74]))
plot.plot([(i + 1) / 100 for i in range(100)], [values[74] for i in range(100)], alpha=0.3)

plot.scatter(95, values[94], color="red")
plot.annotate("95%: " + str(int(values[94])), (0, values[94]))
plot.plot([(i + 1) / 100 for i in range(100)], [values[94] for i in range(100)], alpha=0.3)



plot.show()