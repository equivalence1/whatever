#pragma once

#include <cxxopts.hpp>

#include <string>


struct TBuilderOptions final {
    std::string ModelPath;
    std::string OutputEnginePath;
};

struct TExecutorOptions final {
    std::string EnginePath;
};

struct TAppOptions final {
    std::string App;
    TBuilderOptions BuilderOptions;
    TExecutorOptions ExecutorOptions;
};


void AddOptions(cxxopts::Options* options);
TAppOptions ParseOptions(const cxxopts::ParseResult& parsedOptions);
