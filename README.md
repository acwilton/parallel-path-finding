# Parallel Path-Finding
An investigation into optimizing parallel path-finding algorithms in a gridded environment.

The project consists of many individual executables that can be run individually or through use of the graphical program that is apart of this project.

All routing is done through a gridded enviornment called a "world." a world can be generated through the worldgen executable and will be placed under a worlds/ folder (it has to exist beforehand for this to work). The parameters for the world generator is <name of world> <width> <height> optional:(max tile cost). All generated worlds are gaurenteed to be continuous and the cost of each tile is a random number in the range [1, max tile cost], with the default for max tile cost being 255. The range for max tile cost should be a number from 1 to 255.

You can view the worlds through the gui by clicking the view world button and typing the name you gave to the world file. You can also generate a world with the gui button by typing the parameters for the world generator just as you would on the command line.

When you run an algorithm, you must enter the name of the world  followed by the start x and y and the end x and y. Assumming these don't land out of bounds or on a wall, then the routing algorithm will run and push it's results into a new folder underneath the results/ folder. Two files will be generated. algorithm.perf and algorithm.res. The .perf file will give performance metrics of the the algorithm for that particular run (currently just execution time). The .res file will be a file containing the path taken and the total cost of the path.

## Current algorithms implemented:
+ Dijskstra
+ A*
+ Bidirectional A*
+ Parallel Bidirectional A*
+ Fringe Search
