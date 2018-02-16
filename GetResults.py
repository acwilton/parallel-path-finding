import os
import time


class Size:
    def __init__(self, width, height):
        self.width = str(width)
        self.height = str(height)

    def getStr(self):
        return self.width + "x" + self.height


numForwRuns = 1
numBackRuns = 2
worldPerSize = 2
sizes = [Size(100, 100)]  # [Size(100, 100), Size(1000, 1000), Size(10000, 10000)]
costs = [1, 255]
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
    for i in range(0, worldPerSize):
        worldName = baseWorldName + "_" + str(i)
        runCommand("./worldGen " + worldName + " " + size.width
                   + " " + size.height + " " + costStr)

        for algorithm in algorithms:
            for j in range(0, numForwRuns):
                runAlgorithm(algorithm, worldName)
            for j in range(0, numBackRuns):
                runAlgorithm(algorithm, worldName, True)


def runAlgorithm(algorithm, world, reverse=False):
    command = "./" + algorithm + " " + world
    if reverse:
        with open("worlds/" + world + ".path") as pathFile:
            startX = pathFile.readline().strip()
            startY = pathFile.readline().strip()
            endX = pathFile.readline().strip()
            endY = pathFile.readline().strip()
            # print(startX, " ", startY, " ", endX, " ", endY)
            command += " " + endX + " " + endY + " " + startX + " " + startY

    runCommand(command)


def runCommand(command):
    print(time.asctime() + ": " + command)
    os.system(command)


os.chdir("build")
for cost in costs:
    for size in sizes:
        testWorld(size, cost)
os.chdir("..")
