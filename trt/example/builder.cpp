#include "builder.h"
#include "utils.h"
#include "options_binder.h"

#include <NvInfer.h>
#include <NvOnnxParser.h>

#include <iostream>
#include <fstream>
#include <memory>
#include <stdexcept>


void BuildEngine(const TBuilderOptions& options) {
    TLogger logger;

    std::unique_ptr<nvinfer1::IBuilder> builder(nvinfer1::createInferBuilder(logger));
    ENSURE(builder);

    std::cout << "created builder" << std::endl;

    // Create network definition
    const uint32_t flag = 1U << static_cast<uint32_t>(nvinfer1::NetworkDefinitionCreationFlag::kEXPLICIT_BATCH);
    std::unique_ptr<nvinfer1::INetworkDefinition> network(builder->createNetworkV2(flag));
    ENSURE(network);

    std::cout << "created network definition" << std::endl;

    // std::cout << "known network inputs (" << (network->getNbInputs()) << ") :" << std::endl;
    // for (int32_t i = 0; i < network->getNbInputs(); ++i) {
    //     std::cout << network->getInput(i)->getName() << std::endl;
    // }
    // 
    // network->addInput("features", nvinfer1::DataType::kFLOAT, nvinfer1::Dims3(-1, -1, 192));
    // network->addOutput("output_features", nvinfer1::DataType::kFLOAT, nvinfer1::Dims3(-1, -1, 192));
    // network->addOutput("pitch", nvinfer1::DataType::kFLOAT, nvinfer1::Dims3(-1, -1));
    // network->addOutput("durations", nvinfer1::DataType::kFLOAT, nvinfer1::Dims3(-1, -1));
    // 
    // std::cout << "known network inputs (" << (network->getNbInputs()) << ") :" << std::endl;
    // for (int32_t i = 0; i < network->getNbInputs(); ++i) {
    //     std::cout << network->getInput(i)->getName() << std::endl;
    // }

    // Parse onnx model
    std::unique_ptr<nvonnxparser::IParser> parser(nvonnxparser::createParser(*network, logger));
    ENSURE(parser);

    std::cout << "built parser" << std::endl;

    parser->parseFromFile(options.ModelPath.c_str(), static_cast<int32_t>(nvinfer1::ILogger::Severity::kWARNING));
    for (int32_t i = 0; i < parser->getNbErrors(); ++i) {
        std::cerr << parser->getError(i)->desc() << std::endl;
    }

    std::unique_ptr<nvinfer1::IBuilderConfig> config(builder->createBuilderConfig());
    ENSURE(config);
    config->setMaxWorkspaceSize(1U << 25);

    nvinfer1::IOptimizationProfile* profile = builder->createOptimizationProfile();
    ENSURE(profile);
    // TODO make it a part of config
    profile->setDimensions("features", nvinfer1::OptProfileSelector::kMIN, nvinfer1::Dims3(3,100,192));
    profile->setDimensions("features", nvinfer1::OptProfileSelector::kOPT, nvinfer1::Dims3(3,150,192));
    profile->setDimensions("features", nvinfer1::OptProfileSelector::kMAX, nvinfer1::Dims3(3,200,192));
    config->addOptimizationProfile(profile);

    std::cout << "built config" << std::endl;

    std::unique_ptr<nvinfer1::ICudaEngine> engine(builder->buildEngineWithConfig(*network, *config));
    ENSURE(engine);

    std::cout << "built engine" << std::endl;

    std::unique_ptr<nvinfer1::IHostMemory> serializedModel(engine->serialize());
    ENSURE(serializedModel);

    // std::unique_ptr<nvinfer1::IHostMemory> serializedModel(builder->buildSerializedNetwork(*network, *config));

    std::cout << "built serializedModel" << std::endl;

    std::ofstream engineFile(options.OutputEnginePath, std::ios::binary | std::ios::out);
    engineFile.write(reinterpret_cast<const char*>(serializedModel->data()), serializedModel->size());
    engineFile.close();

    std::cout << "wrote to file" << std::endl;
}
