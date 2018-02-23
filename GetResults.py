import os
import glob
import time
import numpy as np
import matplotlib.mlab as mlab
import matplotlib.pyplot as plt


class Size:
    def __init__(self, width, height):
        self.width = str(width)
        self.height = str(height)

    def getStr(self):
        return self.width + "x" + self.height


numRuns = 6
worldPerSize = 3
sizes = [Size(10000, 10000)]# [Size(100, 100), Size(1000, 1000), Size(10000, 10000)]
costs = [5, 25, 75, 175]
algorithms = ["dijkstra",
              "aStar",
              "bidir",
              "parBidir",
              "fringe",
              "parFringe_2",
              "parFringe_4",
              "parFringe_6",
              "parFringe_8",
              "parDivideUnsmooth_4",
              "parDivideUnsmooth_6",
              "parDivideUnsmooth_8",
              "parDivide_4",
              "parDivide_6",
              "parDivide_8"]
worldNamePre = "run"


"""
class Path:
    class Point:
        def __init__(self, x=0, y=0):
            self.x = x
            self.y = y

    def __init__(self, start=Point(), end=Point()):
        self.start = start
        self.end = end
"""


def testWorld(size, cost):
    costStr = str(cost)
    baseWorldName = worldNamePre + "_" + size.getStr() + "_" + costStr

    finalResultsDirName = "compiledResults/" + baseWorldName
    if not os.path.exists(finalResultsDirName):
        os.makedirs(finalResultsDirName)

    for i in range(0, worldPerSize):
        worldName = baseWorldName + "_" + str(i)
        runCommand("./worldGen " + worldName + " " + size.width
                   + " " + size.height + " " + costStr)

        for algorithm in algorithms:
            perfs = np.zeros(numRuns, dtype=int)
            dists = np.zeros(numRuns, dtype=int)
            costs = np.zeros(numRuns, dtype=int)

            with open(finalResultsDirName + "/" + algorithm + ".data",
                      'a') as dataFile:
                for j in range(0, numRuns):
                    perfs[j], dists[j], costs[j] = runAlgorithm(algorithm,
                                                                worldName)
                    # Append the stats for this run to the dataFile
                    dataFile.write(str(perfs[j]) + " " + str(dists[j]) + " "
                                   + str(costs[j]) + "\n")
            with open(finalResultsDirName + "/" + algorithm + ".stddata",
                      'a') as runFile:
                runFile.write(str(perfs.std()) + " " + str(dists.std())
                              + " " + str(costs.std()) + "\n")

    meanPerfs = np.zeros(len(algorithms), dtype=float)
    distPerfs = np.zeros(len(algorithms), dtype=float)
    costPerfs = np.zeros(len(algorithms), dtype=float)
    with open(finalResultsDirName + "/stats.txt", 'w') as statsFile:
        for i in range(0, len(algorithms)):
            algorithm = algorithms[i]

            # Converts algorithm data file into array of arrays with format:
            # [[perf, dist, cost], ...]
            algData = np.array([np.fromstring(line.strip(), dtype=int, sep=' ')
                               for line in open(finalResultsDirName + "/"
                                                + algorithm + ".data")])

            stdData = np.array([np.fromstring(line.strip(),
                                              dtype=float, sep=' ') for line in
                                open(finalResultsDirName + "/" + algorithm
                                     + ".stddata")])

            # Splits the data into seperate arrays for each type of data
            perfData = algData[np.arange(len(algData)), 0]
            distData = algData[np.arange(len(algData)), 1]
            costData = algData[np.arange(len(algData)), 2]

            perfStdData = stdData[np.arange(len(stdData)), 0]
            distStdData = stdData[np.arange(len(stdData)), 1]
            costStdData = stdData[np.arange(len(stdData)), 2]

            statsFile.write(algorithm + "\n")
            statsFile.write("".join(['-']*len(algorithm)) + "\n")
            statsFile.write("Number of runs: " + str(len(algData)) + "\n\n")

            statsFile.write("Average execution time: " + str(perfData.mean())
                            + "\n")
            statsFile.write("Std execution time: " + str(perfStdData.mean())
                            + "\n\n")

            statsFile.write("Average path cost: " + str(costData.mean())
                            + "\n")
            statsFile.write("Std path cost: " + str(costStdData.mean())
                            + "\n\n")

            statsFile.write("Average distance: " + str(distData.mean())
                            + "\n")
            statsFile.write("Std distance: " + str(distStdData.mean())
                            + "\n\n")

            statsFile.write("\n")

            meanPerfs[i] = perfData.mean()
            distPerfs[i] = distData.mean()
            costPerfs[i] = costData.mean()

    # Setup plot
    figure, (perfAx, costAx, distAx) = plt.subplots(sharey='row', ncols=3)

    indexes = np.arange(len(algorithms))

    perfAx.barh(indexes, meanPerfs)
    perfAx.set_yticks(indexes)
    perfAx.set_yticklabels(algorithms)

    costAx.barh(indexes, costPerfs)

    distAx.barh(indexes, distPerfs)

    figure.tight_layout()
    figure.set_figwidth(12.8)
    figure.savefig(finalResultsDirName + "/graphs.png")


def runAlgorithm(algorithm, world):
    command = "./" + algorithm + " " + world
    runCommand(command)

    startX = ""
    startY = ""
    endX = ""
    endY = ""
    with open("worlds/" + world + ".path") as pathFile:
        startX = pathFile.readline().strip()
        startY = pathFile.readline().strip()
        endX = pathFile.readline().strip()
        endY = pathFile.readline().strip()

    algorithmFolderName = "results/" + world + "_" + startX + "_" + startY \
        + "_" + endX + "_" + endY

    perf = 0
    dist = 0
    cost = 0
    with open(algorithmFolderName + "/" + algorithm + ".perf") as perfFile:
        perfFile.seek(6)
        perf = int(perfFile.readline().strip())

    with open(algorithmFolderName + "/" + algorithm + ".res", 'rb') as resFile:
        dist = int(resFile.readline().strip())
        resFile.seek(-5, os.SEEK_END)
        while resFile.read(1) != b'\n':
            resFile.seek(-2, os.SEEK_CUR)
        resFile.seek(12, os.SEEK_CUR)
        cost = int(resFile.readline().decode())

    return (perf, dist, cost)


def runCommand(command):
    print(time.asctime() + ": " + command)
    os.system(command)


os.chdir("build")
for cost in costs:
    for size in sizes:
        testWorld(size, cost)
os.chdir("..")
