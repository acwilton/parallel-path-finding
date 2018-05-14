import numpy as np
import os
import matplotlib
import matplotlib.pyplot as plt

algorithms = ["dijkstra",
              "aStar",
              "bidir",
              "parBidir",
              "fringe",
              "parFringe_8",
              "parDivide_8"]


algorithmLabels = ["Dijkstra",
                   "A*",
                   "BA*",
                   "PBA*",
                   "FS",
                   "DFS",
                   "PDS"]


def ResultGraph(location, graphName, algorithms, algorithmLabels, graphTitle):
    meanPerfs = np.zeros(len(algorithms), dtype=float)
    meanDists = np.zeros(len(algorithms), dtype=float)
    meanCosts = np.zeros(len(algorithms), dtype=float)

    for i in range(0, len(algorithms)):
        algorithm = algorithms[i]

        # Converts algorithm data file into array of arrays with format:
        # [[perf, dist, cost], ...]
        algData = np.array([np.fromstring(line.strip(), dtype=int, sep=' ')
                           for line in open(location + "/"
                                            + algorithm + ".data")])

        # Splits the data into seperate arrays for each type of data
        perfData = algData[np.arange(len(algData)), 0]
        distData = algData[np.arange(len(algData)), 1]
        costData = algData[np.arange(len(algData)), 2]

        meanPerfs[i] = int(round(perfData.mean(), 0))
        meanDists[i] = int(round(distData.mean(), 0))
        meanCosts[i] = int(round(costData.mean(), 0))

    # Make Graph
    figure, perfAx = plt.subplots()

    indexes = np.arange(len(algorithmLabels))

    figure.set_size_inches(8, 8)
    perfAx.set_title(
        graphTitle, y=1.01,
        size='x-large')

    perfAx.set_xlabel('Algorithms', labelpad=10.0, size='large')
    perfAx.set_xticks(indexes)
    perfAx.set_xticklabels(algorithmLabels)
    plt.setp(perfAx.get_xticklabels(), rotation=0)

    costAx = perfAx.twinx()

    barWidth = 0.35
    barColor = 'LightBlue'
    labelColor = 'SteelBlue'
    perfRects = perfAx.bar(indexes - barWidth/2, meanPerfs, barWidth,
                           color=barColor,
                           label='Average Execution Time (ms)')
    perfAx.set_ylabel('Average Execution Time (ms)', color=labelColor,
                      labelpad=10.0, size='large')
    perfAx.tick_params(axis='y', labelcolor=labelColor)

    for rect in perfRects:
        height = format(int(round(rect.get_height(), 0)))
        yPos = rect.get_height() + perfAx.get_ylim()[1] * .01
        perfAx.text(rect.get_x() + rect.get_width()/2,
                    yPos,
                    height, rotation=90,
                    ha='center', va='bottom')

    barColor = 'LightCoral'
    labelColor = 'DarkRed'
    costRects = costAx.bar(indexes + barWidth/2, meanCosts, barWidth,
                           color=barColor, label='Average Path Cost')
    costAx.set_ylabel('Average Path Cost', color=labelColor, labelpad=20.0,
                      size='large')
    costAx.tick_params(axis='y', labelcolor=labelColor)
    costAx.set_ybound(meanCosts.min() - meanCosts.min() * .1,
                      meanCosts.max() + meanCosts.min() * .05)
    costAx.axhline(meanCosts.min(), dashes=(1.0, 1.0), color=labelColor)

    costAx.get_yaxis().set_major_formatter(
        matplotlib.ticker.FuncFormatter(lambda x, p: format(int(x), ',')))

    for rect in costRects:
        height = '{:,}'.format(int(round(rect.get_height(), 0)))
        costAx.text(rect.get_x() + rect.get_width()/2,
                    rect.get_height() + costAx.get_ylim()[1] * .0025,
                    height, rotation=90,
                    ha='center', va='bottom')

    figure.tight_layout()
    figure.savefig(location + "/" + graphName + ".png")


def CustomGraph(location, graphName, algorithms, algorithmLabels, graphTitle):
    meanPerfs = np.zeros(len(algorithms), dtype=float)
    meanDists = np.zeros(len(algorithms), dtype=float)
    meanCosts = np.zeros(len(algorithms), dtype=float)

    for i in range(0, len(algorithms)):
        algorithm = algorithms[i]

        # Converts algorithm data file into array of arrays with format:
        # [[perf, dist, cost], ...]
        algData = np.array([np.fromstring(line.strip(), dtype=int, sep=' ')
                           for line in open(location + "/"
                                            + algorithm + ".data")])

        # Splits the data into seperate arrays for each type of data
        perfData = algData[np.arange(len(algData)), 0]
        distData = algData[np.arange(len(algData)), 1]
        costData = algData[np.arange(len(algData)), 2]

        meanPerfs[i] = round(perfData.mean() / 1000, 3)
        meanDists[i] = int(round(distData.mean(), 0))
        meanCosts[i] = int(round(costData.mean(), 0))

    # Make Graph
    figure, perfAx = plt.subplots()

    indexes = np.arange(len(algorithmLabels))

    figure.set_size_inches(len(indexes) + 4, 6)
    perfAx.set_title(
        graphTitle, y=1.01,
        size='x-large')

    perfAx.set_xlabel('Algorithms', labelpad=10.0, size='large')
    perfAx.set_xticks(indexes)
    perfAx.set_xticklabels(algorithmLabels)
    plt.setp(perfAx.get_xticklabels(), rotation=0)

    costAx = perfAx.twinx()

    maxPerf = 10

    barWidth = 0.35
    barColor = 'LightBlue'
    labelColor = 'SteelBlue'

    perfRects = perfAx.bar(indexes - barWidth/2, meanPerfs, barWidth,
                           color=barColor,
                           label='Average Execution Time (sec)')
    perfAx.set_ylabel('Average Execution Time (sec)', color=labelColor,
                      labelpad=10.0, size='large')
    perfAx.set_ybound(0, maxPerf)
    perfAx.tick_params(axis='y', labelcolor=labelColor)

    perfAx.get_yaxis().set_major_formatter(
        matplotlib.ticker.FuncFormatter(lambda x, p: format(int(x), ',')))

    for rect in perfRects:
        height = '{:,}'.format(rect.get_height())

        if (rect.get_height() < maxPerf):
            va = 'bottom'
            xPos = rect.get_x() + rect.get_width()/2
            yPos = rect.get_height() + perfAx.get_ylim()[1] * .01
        else:
            va = 'top'
            xPos = rect.get_x() + rect.get_width()/2
            yPos = maxPerf - perfAx.get_ylim()[1] * .01

        perfAx.text(xPos, yPos, height, rotation=90, ha='center', va=va)


    barColor = 'LightCoral'
    labelColor = 'DarkRed'
    costRects = costAx.bar(indexes + barWidth/2, meanCosts, barWidth,
                           color=barColor, label='Average Path Cost')
    costAx.set_ylabel('Average Path Cost', color=labelColor, labelpad=20.0,
                      size='large')
    costAx.tick_params(axis='y', labelcolor=labelColor)
    costAx.set_ybound(meanCosts.min() - meanCosts.min() * .05,
                      meanCosts.max() + meanCosts.min() * .05)
    costAx.axhline(meanCosts.min(), dashes=(1.0, 1.0), color=labelColor)

    costAx.get_yaxis().set_major_formatter(
        matplotlib.ticker.FuncFormatter(lambda x, p: format(int(x), ',')))

    for rect in costRects:
        height = '{:,}'.format(int(round(rect.get_height(), 0)))
        costAx.text(rect.get_x() + rect.get_width()/2,
                    rect.get_height() + costAx.get_ylim()[1] * .0025,
                    height, rotation=90,
                    ha='center', va='bottom')

    figure.tight_layout()
    # figure.set_figwidth(12.8)
    figure.savefig(location + "/" + graphName + ".png")
    # plt.show()
