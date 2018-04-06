import numpy as np
import os
import matplotlib
import matplotlib.pyplot as plt


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
    #perfAx.set_ybound(0, 120)

    for rect in perfRects:
        height = format(int(round(rect.get_height(), 0)))
        #if (rect.get_height() < 108):
        yPos = rect.get_height() + perfAx.get_ylim()[1] * .01
        #else:
            #yPos = 108
            # perfAx.arrow(rect.get_x() + rect.get_width(), yPos, 0, 10000)
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
    # figure.set_figwidth(12.8)
    figure.savefig(location + "/" + graphName + ".png")
    # plt.show()


"""
    figure, perfAx = plt.subplots()

    indexes = np.arange(len(algorithmLabels)) * 2

    perfAx.set_title(
        'Algorithm Performances Over a\n10,000 x 10,000 Size Map', y=1.0)
    perfAx.set_ylabel('Algorithms')
    perfAx.set_yticks(indexes)
    perfAx.set_yticklabels(algorithmLabels)

    costAx = perfAx.twiny()

    barWidth = 0.5
    barColor = 'SkyBlue'
    perfRects = perfAx.barh(indexes - barWidth/2, meanPerfs, barWidth,
                            color=barColor,
                            label='Average Execution Time (ms)')
    perfAx.set_xlabel('Average Execution Time (ms)', color=barColor,
                      labelpad=20.0)
    perfAx.tick_params(axis='x', labelcolor=barColor)
    plt.setp(perfAx.get_xticklabels(), rotation=30,
             horizontalalignment='right')

    for rect in perfRects:
        width = '{}'.format(int(round(rect.get_width(), 0)))
        perfAx.text(rect.get_width() + perfAx.get_xlim()[1] * .01,
                    rect.get_y() + rect.get_height()/2, width,
                    ha='left', va='center')

    barColor = 'IndianRed'
    costRects = costAx.barh(indexes + barWidth/2, meanCosts, barWidth,
                            color=barColor, label='Average Path Cost')
    costAx.set_xlabel('Average Path Cost', color=barColor, labelpad=20.0)
    costAx.tick_params(axis='x', labelcolor=barColor)
    costAx.set_xbound(meanCosts.min() - meanCosts.min() * .1,
                      meanCosts.max() + meanCosts.min() * .1)
    plt.setp(costAx.get_xticklabels(), rotation=-30,
             horizontalalignment='right')

    for rect in costRects:
        width = '{}'.format(int(round(rect.get_width(), 0)))
        costAx.text(rect.get_width() + perfAx.get_xlim()[1] * .01,
                    rect.get_y() + rect.get_height()/2, width,
                    ha='left', va='center')

    figure.tight_layout()
    figure.set_figwidth(12.8)
    # figure.savefig(location + "/" + graphName + ".png")
    plt.show()
    """
