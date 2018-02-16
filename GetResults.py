import os
import time
import ResultsConfig
numForwRuns = ResultsConfig.numForwRuns
numBackRuns = ResultsConfig.numBackRuns
worldPerSize = ResultsConfig.worldPerSize
sizes = ResultsConfig.sizes
costs = ResultsConfig.costs


WORLD_NAME = "run"


def testWorld(width, height, cost):
    widthStr = str(width)
    heightStr = str(height)
    costStr = str(cost)
    baseWorldName = "run_" + widthStr + "x" + heightStr + "_" + costStr
    for i in range(0, worldPerSize):
        worldName = baseWorldName + "_" + str(i)

        worldGenCommand = "./worldGen " + worldName + " " + widthStr \
            + " " + heightStr + " " + costStr
        print(time.asctime() + ": " + worldGenCommand)
        os.system(worldGenCommand)


os.chdir("build")
for cost in costs:
    for size in sizes:
        testWorld(size[0], size[1], cost)
os.chdir("..")
