#include "options_binder.h"

#include <cxxopts.hpp>


namespace {

    TBuilderOptions ParseBuilderOptions(const cxxopts::ParseResult& parsedOptions) {
        TBuilderOptions result;
        if (parsedOptions.count("model_path")) {
            result.ModelPath = parsedOptions["model_path"].as<std::string>();
        }
        if (parsedOptions.count("output_engine_path")) {
            result.OutputEnginePath = parsedOptions["output_engine_path"].as<std::string>();
        }
        return result;
    }

    TExecutorOptions ParseExecutorOptions(const cxxopts::ParseResult& parsedOptions) {
        TExecutorOptions result;
        if (parsedOptions.count("engine_path")) {
            result.EnginePath = parsedOptions["engine_path"].as<std::string>();
        }
        return result;
    }

    void AddBuilderOptions(cxxopts::Options* options) {
        options->add_options()
            ("model_path", "Path to original model for builder app", cxxopts::value<std::string>())
            ("output_engine_path", "Path to output trt engine", cxxopts::value<std::string>());
    }

    void AddExecutorOptions(cxxopts::Options* options) {
        options->add_options()
            ("engine_path", "Path to original model for executor app", cxxopts::value<std::string>());
    }

}

TAppOptions ParseOptions(const cxxopts::ParseResult& parsedOptions) {
    TAppOptions result;

    result.App = parsedOptions["app"].as<std::string>();

    result.BuilderOptions = ParseBuilderOptions(parsedOptions);
    result.ExecutorOptions = ParseExecutorOptions(parsedOptions);

    return result;
}

void AddOptions(cxxopts::Options* options) {
    // Positional arguments? I don't think this lib handles all I need, maybe should write my own parser...
    options->add_options()
            ("app", "Application to run (builder|executor)", cxxopts::value<std::string>())
            ("h,help", "Print usage");

    AddBuilderOptions(options);
    AddExecutorOptions(options);
}
