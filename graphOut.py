import matplotlib.pyplot as plt
import sys
import pandas as pd
import numpy as np
import math
import time

values = []

parallel = False
save_to_csv = False

totalCowns = 0
cownDist = ""
behaviourCowns = 0
whenCount = 0
serviceTime = ""

# Need to implement argument storage for the csv file. 
# If arguments are included, this value is true and we don't output a graph. 
# Want to be able to create a csv file and immediately return, 
# so that our test returns and we can run the next automatically.
# Should do this now, since it is going to help in the interview (CSV FILES)

for i in range(13):
    if (sys.argv[i] == "--csv"):
        save_to_csv = True
    elif (sys.argv[i] == "--cownPop"):
        cownDist = sys.argv[i+1]
    elif (sys.argv[i] == "--servTime"):
        serviceTime = sys.argv[i+1]
    elif (sys.argv[i] == "--totalCowns"):
        totalCowns = sys.argv[i+1]
    elif (sys.argv[i] == "--behaviourCowns"):
        behaviourCowns = sys.argv[i+1]
    elif (sys.argv[i] == "--whenCount"):
        whenCount = sys.argv[i+1]
    elif (sys.argv[i] == "--parallel"):
        parallel = True
    
if save_to_csv:
    timeVals = sys.argv[13:]
else:
    timeVals = sys.argv[1:]

print(timeVals)

for i in timeVals:
    values.append((float(i)))

x = [(i+1) / len(values) for i in range(len(values))]
values = sorted(values)

throughputList = [0] * (math.ceil(max(values)) + 2)
currentTime = 1
for i in values:
    if i >= currentTime:
        currentTime += 1
    if parallel:
        throughputList[currentTime] += 0.5
    else:
        throughputList[currentTime] += 1

avgThroughput = len(values) / values[-1]
maxThroughput = max(throughputList)

if parallel:
    avgThroughput /= 2

# print(throughputList)

total_count = len(values)
_5 = values[int((total_count / 100) * 5 - 1)]
_25 = values[int((total_count / 100) * 25 - 1)]
_50 = values[int((total_count / 100) * 50 - 1)]
_75 = values[int((total_count / 100) * 75 - 1)]
_95 = values[int((total_count / 100) * 95 - 1)]

data = {
    "cowns": [totalCowns],
    "cownDist": [cownDist],
    "behaviourCowns": [behaviourCowns],
    "whenCount": [whenCount],
    "serviceTime": [serviceTime],
    "parallel": [parallel],
    "5%": [_5],
    "25%": [_25],
    "50%": [_50],
    "75%": [_75],
    "95%": [_95],
    "max": [maxThroughput],
    "avg": [avgThroughput]
}

df = pd.DataFrame(data)
print(df)

df.to_csv(path_or_buf="/Users/ryanward/Documents/git_repos/verona-rt/data.csv", mode="a", index=False, header=False)

fig, (plot, plot2) = plt.subplots(2)

plot2.plot([i for i in range(len(throughputList))], throughputList)
plot2.axhline(y = avgThroughput, color="r")


plt.rcParams.update({"font.size": 6})

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


# print("5%", "at ", values[int((total_count / 100) * 5 - 1)], 'seconds')
# print("25%", "at ", values[int((total_count / 100) * 25 - 1)], 'seconds')
# print("50%", "at ", values[int((total_count / 100) * 50 - 1)], 'seconds')
# print("75%", "at ", values[int((total_count / 100) * 75 - 1)], 'seconds')
# print("95%", "at ", values[int((total_count / 100) * 95 - 1)], 'seconds')

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

fname = "/Users/ryanward/Documents/git_repos/verona-rt/figures/"


fname += "cowns_" + str(totalCowns) + "__cownDist_" + str(cownDist) + "__behaviourCount_" + str(behaviourCowns) + "__whenCount_" + str(whenCount) + "__serviceTime_" + str(serviceTime) + "__parallel_" + str(parallel) + ".png"

plt.gcf().set_size_inches(12, 12)
plt.savefig(fname, dpi=400)

plt.close