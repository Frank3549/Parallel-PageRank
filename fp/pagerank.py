import sys
from operator import add, itemgetter
import time

from pyspark.sql import SparkSession

BETA = 0.80

def parse_edge(line):
    vertices = line.split()
    return (int(vertices[0]), int(vertices[1]))


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Missing one or more required arguments", file=sys.stderr)
        print("Usage: pagerank <file> <iterations>", file=sys.stderr)
        sys.exit(-1)

    # Extract command line arguments
    filename = sys.argv[1]
    iterations = int(sys.argv[2])

    # Create spark session using "new" SparkSession API
    spark = SparkSession.builder.appName("PageRank").getOrCreate()
    spark.sparkContext.setLogLevel("WARN") # Reduce logging output

  

    # Load the file creating an RDD of edge pairs, i.e. (source_vertex, dest_vertex), while
    # filtering out any duplicate edges (using distinct)
    edges = spark.sparkContext.textFile(filename).map(parse_edge).distinct()

    # Determine the number of vertices
    vertices = edges.flatMap(lambda x: x).distinct()
    num_vertices = vertices.count()

    # TODO: Implement iterative PageRank algorithm, printing out the top-ten highest ranked nodes in
    # descending order of rank, e.g.
    # 173:    0.01673849720556171
    # 618:    0.016584381121521937
    # ...


    
    # Compute the out-degree of each vertex to find L(i) 
    # Output: (from_node, total number of out_links)
    out_degrees = edges.map(lambda edge: (edge[0], 1)).reduceByKey(add)

    # Join each edge with its source node's out-degree
    # Output: (from_node, (to_node, out_degree of from_node))
    edges_with_degree = edges.join(out_degrees)
    
    # Output: (to_node, (from_node, 1 / out_degree of from_node))
    M = edges_with_degree.map(lambda x: (x[1][0], (x[0], 1.0 / x[1][1]) )).cache()
    M.count() # Force Evaluation of M before the timer starts.


    # Rank Vector setup with initial rank of (1.0 - BETA) / num_vertices
    R = vertices.map(lambda x: (x, (1.0 - BETA) / num_vertices))

    # Time execution
    begin_time = time.time()
    
    for i in range(iterations):        
        # Collect the ranks to a dictionary and broadcast it
        R_bcast = spark.sparkContext.broadcast(dict(R.collect()))

        # Output: (to_node, contribution value)
        contributions = M.map(lambda x: (x[0], R_bcast.value.get(x[1][0]) * x[1][1]))
        new_rank = contributions.reduceByKey(add) 

        # Apply the PageRank formula
        R = new_rank.mapValues(lambda rank: BETA * rank + (1.0 - BETA) / num_vertices)

    # Sort the ranks in descending order and take the top 10
    sorted_top_10 = R.takeOrdered(10, key=lambda x: -x[1])

    for node, rank in sorted_top_10:
        print(f"{node}:\t{rank}")


    end_time = time.time()
    print(f"Total program time: {(end_time-begin_time):.2f} seconds")

    spark.stop()
