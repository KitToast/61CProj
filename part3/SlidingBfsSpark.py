from pyspark import SparkContext
import Sliding, argparse


def bfs_reduce(level1, level2):    
    return min(level1, level2)

def bfs_flatmap(board):
    if board[1] == (level - 1):
        children_list = Sliding.children(WIDTH, HEIGHT, board[0])
        level_list = [level for _ in range(len(children_list))]
        children_list = zip(children_list, level_list)
        children_list.append(board)
        return children_list
    return [board]

def flip_map(board):
    return (board[1], board[0])

def flip_reduce(board1, board2):
    return board1

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
    global HEIGHT, WIDTH, level

    # Initialize global constants
    HEIGHT=height
    WIDTH=width
    level = 1 # this "constant" will change, but it remains constant for every MapReduce job

    # The solution configuration for this sliding puzzle. You will begin exploring the tree from this node
    sol = [ ( Sliding.solution(WIDTH, HEIGHT), 0 ) ]
    sol_rdd = sc.parallelize(sol)
   
    prev_lvl_count = 0
    current_lvl_count = 1
    current_lvl_rdd = sol_rdd

    """ YOUR MAP REDUCE PROCESSING CODE HERE """
    while prev_lvl_count != current_lvl_count:
        current_lvl_rdd = current_lvl_rdd.flatMap(bfs_flatmap).reduceByKey(bfs_reduce)
        #print(current_lvl_rdd.collect())
        prev_lvl_count = current_lvl_count
        current_lvl_count = current_lvl_rdd.count()
        level += 1
        if level % 8 == 0:
            current_lvl_rdd = current_lvl_rdd.partitionBy(16)

    """ YOUR OUTPUT CODE HERE """

    current_lvl_rdd = current_lvl_rdd.map(flip_map).sortByKey()
    rdd_list = current_lvl_rdd.collect()
    for board in rdd_list:
        output(str(board[0]) + " " + str(board[1]))

""" DO NOT EDIT PAST THIS LINE

You are welcome to read through the following code, but you
do not need to worry about understanding it. """

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
