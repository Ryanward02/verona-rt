import matplotlib.pyplot as plt
import sys
import math

values = []

for i in sys.argv[1:]:
    values.append((float(i)))

x = [(i+1) / len(values) for i in range(len(values))]
values = sorted(values)

throughputList = [0] * (math.ceil(max(values)) + 2)
currentTime = 1
for i in values:
    if i >= currentTime:
        currentTime += 1
    throughputList[currentTime] += 1

avgThroughput = len(values) / values[-1]

print(throughputList)

total_count = len(values)
fig, (plot, plot2) = plt.subplots(2)

plot2.plot([i for i in range(len(throughputList))], throughputList)
plot2.axhline(y = avgThroughput, color="r")
plt.sca(plot2)
plt.yticks([0, max(throughputList), avgThroughput])
plt.ylabel("rolling throughput (behaviours/second)")
plt.xlabel("time (seconds)")


plot.scatter(values, x, facecolors='black',alpha=0.55, s=10)
plot.plot(values, x, alpha=0.1)

plt.sca(plot)
plt.yticks([0, 0.05, 0.25, 0.50, 0.75, 0.95, 1])
plt.xticks()
plt.ylabel("Percentage of behaviours executed")
plt.xlabel("Behaviour Latency (Seconds)")


print("5%", "at ", values[int((total_count / 100) * 5 - 1)], 'seconds')
print("25%", "at ", values[int((total_count / 100) * 25 - 1)], 'seconds')
print("50%", "at ", values[int((total_count / 100) * 50 - 1)], 'seconds')
print("75%", "at ", values[int((total_count / 100) * 75 - 1)], 'seconds')
print("95%", "at ", values[int((total_count / 100) * 95 - 1)], 'seconds')

plot.scatter(values[int((total_count / 100) * 5 - 1)], 0.05, color="red")
plot.plot([values[int((total_count / 100) * 5 - 1)] for i in range(100)], [0.04 for i in range(100)], alpha=0.3)

plot.scatter(values[int((total_count / 100) * 25 - 1)], 0.25, color="red")
plot.plot([values[int((total_count / 100) * 25 - 1)] for i in range(100)], [0.24 for i in range(100)], alpha=0.3)


plot.scatter(values[int((total_count / 100) * 50 - 1)], 0.50, color="red")
plot.plot([values[int((total_count / 100) * 50 - 1)] for i in range(100)], [0.49 for i in range(100)], alpha=0.3)

plot.scatter(values[int((total_count / 100) * 75 - 1)], 0.75, color="red")
plot.plot([values[int((total_count / 100) * 75 - 1)] for i in range(100)], [0.74 for i in range(100)], alpha=0.3)

plot.scatter(values[int((total_count / 100) * 95 - 1)], 0.95, color="red")
plot.plot([values[int((total_count / 100) * 95 - 1)] for i in range(100)], [0.94 for i in range(100)], alpha=0.3)

plt.show()