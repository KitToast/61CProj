from pyspark import SparkContext
import Sliding, argparse

def bfs_map(board):
    if (board not in already_seen_set_broadcast.value):
        return ( level, [board] )
    return (level, [])

def bfs_reduce(board1, board2):
    if(len(board2) == 1 and board2[0] in board1): #Early duplicate checking when lists are only length one. Prevents further duplicate checking in iterative portion
        return board1
    elif(len(board1) == 1 and board1[0] in board2):
        return board2
    return board1 + board2  

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
    global HEIGHT, WIDTH, level, already_seen_set_broadcast

    # Initialize global constants
    HEIGHT=height
    WIDTH=width
    level = 0 # this "constant" will change, but it remains constant for every MapReduce job
    
    firstlevel = True;
    # The solution configuration for this sliding puzzle. You will begin exploring the tree from this node
    sol = [Sliding.solution(WIDTH, HEIGHT)]
    #Create initial RDD from entry solution point
    sol_rdd = sc.parallelize(sol)
    #Initialization
    already_seen_set = set() #Solution is already seen
    already_seen_set_broadcast = sc.broadcast(already_seen_set)
    current_lvl_dataset = sol_rdd

    while True: #Testing for only level 0 and 1. Onward will require a helper function dealing with Calling MapReduce with Sliding.children 
        current_lvl_result = current_lvl_dataset.map(bfs_map).reduceByKey(bfs_reduce)
        current_lvl_list = current_lvl_result.collect()

        for result in current_lvl_list: #Bottleneck and change to correct format
            for board in current_lvl_list[0][1]:
                output(str(result[0]) + " " + str(board))
        
        if(not current_lvl_list):
            break

        list_of_unique_children = current_lvl_list[0]

        if(len(list_of_unique_children[1]) == 1 and not firstlevel):
            break
        already_seen_set = already_seen_set.union(set(list_of_unique_children[1])) #Update seen set
        already_seen_set_broadcast.unpersist(blocking = True) 
        already_seen_set_broadcast = sc.broadcast(already_seen_set) #update broadcast

        children_boards = []
        for board in list_of_unique_children[1]:
            children_boards.extend(Sliding.children(WIDTH, HEIGHT , board)) #May have to move this to mapping portion or somehow make it parallel. Another MapReduce?
        current_lvl_dataset = sc.parallelize(children_boards)
        level += 1
        firstlevel = False

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
