install.packages("plotrix")
library(plotrix)

FOLDER_ROOT <- "../../"
FOLDER_OUTPUT <- paste0(FOLDER_ROOT, "output/")
suffix <- "_indorder.csv"

disciplines <- c("economics", "finance", "mathematics", "physics", "politics",
                 "probability", "statistics")

for (discipline in disciplines) {
  cat(paste0(discipline, "\n"))
  filepath <- paste0(FOLDER_OUTPUT, discipline, suffix)
  dat <- read.csv(filepath)
  name <- paste0(toupper(substr(discipline, 1, 1)),
                 substr(discipline, 2, nchar(discipline)))

  twoord.plot(dat$pages_crawled, dat$indegree, dat$pages_crawled, dat$graph_order,
              main=name, xlab="Pages Crawled (Vertices Visited)",
              ylab="In-degree of Most Recently Added Vertex",
              rylab="Order of Graph (Vertices Seen)")

  cat("Please identify points of interest\n")
  identify(dat$pages_crawled, dat$indegree, labels = dat$last_page)
  cat("\n")
}