from pyspark import SparkContext
import Sliding, argparse

def bfs_map(board_tup):
    if (board_tup[1] not in already_seen_set_broadcast.value):
        return ( level, [board_tup[1]] )
    return (level, [])

def bfs_reduce(board1, board2):
    return board1 + board2

def children_map(child):
    return (level, child)

def children_flatMap(board):
    return Sliding.children(WIDTH, HEIGHT, board)

def solve_sliding_puzzle(master, output, height, width):
    """
    Solves a sliding puzzle of the provided height and width.
     master: specifies master url for the spark context
     output: function that accepts string to write to the output file
     height: height of puzzle
     width: width of puzzle
    """
    # Set up the spark context. Use this to create your RDD
    sc = SparkContext(master, "python")

    # Global constants that will be shared across all map and reduce instances.
    # You can also reference these in any helper functions you write.
    global HEIGHT, WIDTH, level, already_seen_set_broadcast, four

    # Initialize global constants
    HEIGHT=height
    WIDTH=width
    level = 0 # this "constant" will change, but it remains constant for every MapReduce job
    
    # The solution configuration for this sliding puzzle. You will begin exploring the tree from this node
    sol = [Sliding.solution(WIDTH, HEIGHT)]
    #Create initial RDD from entry solution point
    sol_rdd = sc.parallelize(sol).map(children_map)
    #Initialization
    already_seen_set = set() #Solution is already seen
    already_seen_set_broadcast = sc.broadcast(already_seen_set)
    current_lvl_dataset = sol_rdd
    
    current_lvl_result = current_lvl_dataset.map(bfs_map).reduceByKey(bfs_reduce) #reduce could happen a lot fewer times
    current_lvl_list = current_lvl_result.collect() #collect is also an action, which makes it bad. 

    for result in current_lvl_list: #Bottleneck and change to correct format
        for board in current_lvl_list[0][1]:
            output(str(result[0]) + " " + str(board))
    list_of_unique_children = current_lvl_list[0]
    
    already_seen_set = already_seen_set.union(set(list_of_unique_children[1])) #Update seen set
    already_seen_set_broadcast.unpersist(blocking = True) 
    already_seen_set_broadcast = sc.broadcast(already_seen_set) #update broadcast
    
    children_boards = []
    for board in list_of_unique_children[1]:
        children_boards.extend(Sliding.children(WIDTH, HEIGHT, board))
    current_lvl_dataset = sc.parallelize(children_boards).distinct().map(children_map)
    current_lvl_dataset = current_lvl_dataset.partitionBy(8)
    level += 1
    
    while True:

        current_lvl_result = current_lvl_dataset.map(bfs_map).reduceByKey(bfs_reduce) #reduce could happen a lot fewer times
    	current_lvl_list = current_lvl_result.collect() #collect is also an action, which makes it bad.

        if (not current_lvl_list):
            break

        for result in current_lvl_list: #Bottleneck and change to correct format
            for board in current_lvl_list[0][1]:
                output(str(result[0]) + " " + str(board))

        list_of_unique_children = current_lvl_list[0]

        if(len(list_of_unique_children[1]) == 1):
            break

        already_seen_set = already_seen_set.union(set(list_of_unique_children[1])) #Update seen set
        already_seen_set_broadcast.unpersist(blocking = True) 
        already_seen_set_broadcast = sc.broadcast(already_seen_set) #update broadcast

        level += 1

	current_lvl_dataset = sc.parallelize(list_of_unique_children[1]).flatMap(children_flatMap).distinct().map(children_map)
        if (level % 8) == 0:
            current_lvl_dataset = current_lvl_dataset.partitionBy(8)

    sc.stop()



""" DO NOT EDIT PAST THIS LINE

You are welcome to read through the following code, but you
do not need to worry about understanding it.
"""

def main():
    """
    Parses command line arguments and runs the solver appropriately.
    If nothing is passed in, the default values are used.
    """
    parser = argparse.ArgumentParser(
            description="Returns back the entire solution graph.")
    parser.add_argument("-M", "--master", type=str, default="local[8]",
            help="url of the master for this job")
    parser.add_argument("-O", "--output", type=str, default="solution-out",
            help="name of the output file")
    parser.add_argument("-H", "--height", type=int, default=2,
            help="height of the puzzle")
    parser.add_argument("-W", "--width", type=int, default=2,
            help="width of the puzzle")
    args = parser.parse_args()


    # open file for writing and create a writer function
    output_file = open(args.output, "w")
    writer = lambda line: output_file.write(line + "\n")

    # call the puzzle solver
    solve_sliding_puzzle(args.master, writer, args.height, args.width)

    # close the output file
    output_file.close()

# begin execution if we are running this file directly
if __name__ == "__main__":
    main()
