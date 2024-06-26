devtools::clean_dll()

devtools::load_all()
variables <- read_redatam("dev/AR-BASE EPH FINAL.dic")

# devtools::install()
# variables <- redatam::read_redatam("dev/AR-BASE EPH FINAL.dic")

print(variables)
names(variables)
description(variables)
