#include "options_binder.h"
#include "builder.h"
#include "executor.h"

#include <cxxopts.hpp>


int main(int argc, char* argv[]) {
    cxxopts::Options options("trt_example", "Example app for trt engines build/exec");

    AddOptions(&options);
    auto parsedOptions = options.parse(argc, argv);

    if (parsedOptions.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    TAppOptions appOptions = ParseOptions(parsedOptions);

    if (appOptions.App == "builder") {
        BuildEngine(appOptions.BuilderOptions);
    }
}
