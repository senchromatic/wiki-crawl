from constants import *
from math import exp
import os
import pydot

def save_graph(discipline):
	# load graph from edgelist
	graph = pydot.Dot(graph_type='digraph', ratio=ASPECT_RATIO)
	filepath = FOLDER_OUTPUT + discipline + '_' + FILE_EDGELIST  # e.g. output/mathematics_graph.txt
	with open(filepath) as input:
		lines = input.readlines()
	
	# filter: keep only strong ties
	values = [line.strip().split(',') for line in lines[1:]]
	statistics = map(float, [value[2] for value in values])
	max_score = max(statistics)
	n = len(statistics)
	quantile = sorted(statistics)[int(n * -FRACTION_EDGES)]
	print("Top " + str(100*FRACTION_EDGES) + "% (|E| = " + str(int(n*FRACTION_EDGES)) + ") of " + discipline + "'s TF-IDF statistics: " + str(quantile))
	
	# # populate graph with nodes
	# vertices = set([value[0] for value in values])
	# for vertex in vertices:
	# 	graph.add_node(pydot.Node(vertex))
	
	# populate graph with edges
	for src,dest,tf_idf in values:
		weight = float(tf_idf)
		if weight > quantile:
			relative_weight = exp(weight - max_score)   # == exp(weight) / exp(max_score)
			darkness = int(relative_weight * 255)
			edge_color = "#%02x%02x%02x" % (darkness, darkness, darkness)
			edge = pydot.Edge(src, dest, label=str(round(weight, SIGNIFICANT_DIGITS)), penwidth = relative_weight*MAX_PEN_WIDTH, color=edge_color)
			graph.add_edge(edge)
	
	graph.write_png(FOLDER_VISUALS + SUBFOLDER_STRONG_TIES + discipline + IMAGE_FILE_EXT)

if __name__ == "__main__":
	for discipline in DISCIPLINES:
		save_graph(discipline)


