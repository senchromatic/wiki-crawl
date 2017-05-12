FOLDER_ROOT <- "../../"
FOLDER_OUTPUT <- paste0(FOLDER_ROOT, "output/")
suffix <- "_edgelist.csv"

disciplines <- c("economics", "finance", "mathematics", "physics", "politics",
                 "probability", "statistics")

for (discipline in disciplines) {
  filepath <- paste0(FOLDER_OUTPUT, discipline, suffix)
  dat <- read.csv(filepath)
  
  name <- paste0(toupper(substr(discipline, 1, 1)),
                 substr(discipline, 2, nchar(discipline)),
                 " (|E| = ", nrow(dat), ")")
  
  q99 = quantile(dat$tf_idf, 0.99)
  cat(paste0(name, ", 99th percentile of tf-idf scores: ", q99, "\n"))
  hist(dat$tf_idf, breaks=50, 
       main=name, xlab="TF-IDF",
       xlim=c(min(dat$tf_idf), q99))
}
