from bs4 import BeautifulSoup
from collections import defaultdict
from constants import *
from math import log
import requests

# key of maximum value
def max_dictionary(d):
	return max(d, key=d.get)

# reject false positives
def noisy(term):
	for false_positive in NOISE:
		if false_positive in term:
			return True
	return False

def visit(src, visited, seen, nominations, tf, count):
	visited.add(src)
	terms = set()  # set of terms that appear in this document
	# download webpage
	url = HOST + src
	resp = requests.get(url)
	html = resp.text
	# scrape HTML
	soup = BeautifulSoup(html, "lxml")
	body = soup.find("div", {"id": "bodyContent", "class": "mw-body-content"})  # main body of article
	for hyperlink in body.find_all("a"):
		# extract URL
		wikilink = hyperlink.get("href");
		if not (wikilink and REGEX.match(wikilink)):
			continue
		dest = REGEX.match(wikilink).group(1)
		if noisy(dest):
			continue
		seen.add(dest)
		# add edge (src page, dest page)
		terms.add(dest)
		tf[src][dest] += 1
		if dest in visited:
			continue
		nominations[dest] += 1
	for term in terms:
		count[term] += 1
	del nominations[src]

def crawl(filepath, max_pages):
	base = os.path.splitext(os.path.basename(filepath))[0]  # e.g. mathematics
	print("<" + base + ">")
	prefix = os.path.join(FOLDER_OUTPUT, base + '_')  # e.g. output/mathematics_graph.txt
	with open(filepath) as input:
		lines = input.readlines()

	# initialize forest
	visited = set()  # which vertices have already been crawled
	seen = set()  # vertices which have positive indegree
	nominations = defaultdict(int)  # indegree of an unvisited link, dynamically updated
	tf = defaultdict(lambda: defaultdict(int))  # adjacency list (edge weight = term frequency)
	count = defaultdict(int)  # number of documents in which a term appears
	roots = set()
	for line in lines:
		root = line.strip()
		roots.add(root)
		nominations[root] = 1
		seen.add(root)
		visit(root, visited, seen, nominations, tf, count)

	# visit the most linked to page
	with open(prefix + FILE_INDORDER, 'w') as output:
		output.write(HEADER_ROW_INORDER + '\n')  # header row
		while len(visited) <= max_pages:
			page = max_dictionary(nominations)
			pages_crawled = str(len(visited))
			indegree = str(nominations[page])
			graph_order = str(len(seen))
			print(page + ": indegree = " + indegree + ", graph order = " + graph_order)
			output.write(",".join([pages_crawled, page, indegree, graph_order]) + '\n')
			visit(page, visited, seen, nominations, tf, count)

	# compute in each document, max term frequency of any term
	max_tfs = defaultdict(int)
	for page in tf.keys():
		max_tfs[page] = max(tf[page].values())
	
	# filter: exclude edges connected to forest roots
	for root in roots:
		del tf[root]
	for page in tf.keys():
		for root in roots:
			tf[page].pop(root, None)
	
	# filter: edges only to visited pages
	for page in tf.keys():
		adj = tf[page]
		ingroup = visited.intersection(adj)
		adj = {friend : adj[friend] for friend in ingroup}
		tf[page] = adj

	# compute tf-idf (normalization = NORM_K)
	tf_idf = defaultdict(lambda: defaultdict(float))
	with open(prefix + FILE_EDGELIST, 'w') as output:
		output.write(HEADER_ROW_EDGELIST + '\n')
		for page in tf.keys():
			for term in tf[page].keys():
				raw_freq = float(tf[page][term])
				tf_weight = NORM_K + NORM_K * (raw_freq / max_tfs[page])
				N = float(len(visited))
				idf = log(N / count[term])
				val = tf_weight * idf
				if val <= MIN_TF_IDF:  # filter: minimum tf-idf threshold
					continue
				tf_idf[page][term] = val
				tf_idf_term = str(tf_idf[page][term])
				print("tf-idf(" + page + "->" + term + ") = " + tf_idf_term)
				output.write(",".join([page, term, tf_idf_term]) + '\n')

	print("</" + base + ">\n")

# visit pages from all /disciplines
def download():
	print("\n")
	for file_discipline in FILES_DISCIPLINE:
		crawl(file_discipline, MAX_PAGES)

if __name__ == "__main__":
	filepath = raw_input("Please enter the path to a file containing relative Wikipedia links (e.g. " + FOLDER_ROOT + "disciplines/mathematics.txt): ")
	max_pages = int(input("How many pages would you like to crawl? "))
	crawl(filepath, max_pages)
