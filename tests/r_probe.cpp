#include <RInside.h>
#include <iostream>

int main(int argc, char **argv) {
    try {
        RInside r;
        if (argc != 2) return 2;
        r["probe_directory"] = std::string(argv[1]);
        r.parseEvalQ(R"(
            files <- list.files(probe_directory, pattern='[.]RData$', full.names=TRUE, recursive=TRUE)
            stopifnot(length(files) > 0)
            for (file in files) {
                e <- new.env()
                load(file, envir=e)
                stopifnot(exists('bv.data.list', envir=e, inherits=FALSE))
                d <- e$bv.data.list
                stopifnot(all(c('bv.species', 'bv.dates', 'bv.grid.spec', 'bv.variables') %in% names(d)))
                cat(basename(file), ':', d$bv.species, ' dates=', length(d$bv.dates),
                    ' variables=', paste(names(d$bv.variables), collapse=','), '\n')
                rm(e, d)
                gc()
            }
        )");
        return 0;
    } catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
