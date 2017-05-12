import os
import re
from sys import float_info

MAX_PAGES = 100  # how many pages to visit
HOST = "https://en.wikipedia.org/wiki/"  # prefix
REGEX = re.compile("^/wiki/([^/\.:#]+)$")  # any /wiki/page, excluding {Category:, .ext, #anchor}
NOISE = {"JSTOR", "OCLC", "_Standard_", "dentifier", "hilosoph", "_Press", "Bibcode", "Springer-Verlag", "_Review", "_Dictionary", "ArXiv"}  # for denoising generic data to extract signal

NORM_K = 0.5  # tf weight = K + (1-K) * (tf[page][term]/max{tf[page]})
MIN_TF_IDF = float_info.epsilon  # minimum tf-idf required for "existence" of an edge

FOLDER_ROOT = "../../"
FOLDER_DISCIPLINES = FOLDER_ROOT + "disciplines/"
FOLDER_OUTPUT = FOLDER_ROOT + "output/"
FILE_INDORDER = "indorder.csv"  # pages_crawled,indegree,graph_order
HEADER_ROW_INORDER = "pages_crawled,last_page,indegree,graph_order"
FILE_EDGELIST = "edgelist.csv"  # from,to,tf_idf
HEADER_ROW_EDGELIST = "from,to,tf_idf"

FILES_DISCIPLINE = sorted([os.path.join(FOLDER_DISCIPLINES, filename) for filename in os.listdir(FOLDER_DISCIPLINES)])  # all filepaths (e.g. "disciplines/mathematics.txt") in "disciplines/" folder
DISCIPLINES = sorted([os.path.splitext(os.path.basename(filename))[0] for filename in os.listdir(FOLDER_DISCIPLINES)])  # basenames (e.g. "mathematics.txt")

FOLDER_VISUALS = FOLDER_ROOT + "visuals/"
SUBFOLDER_STRONG_TIES = "strong_ties/"
IMAGE_FILE_EXT = ".png"
SIGNIFICANT_DIGITS = 3
MAX_PEN_WIDTH = 5

FRACTION_EDGES = 0.03  # stricter restriction on edge weight filter for visualization (0.1 => top 10% of tf-idf values)
ASPECT_RATIO = 0.5  # height-to-width of pydot visualization of graph


