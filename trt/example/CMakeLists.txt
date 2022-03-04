cmake_minimum_required(VERSION 3.8)

project(trt_example)

set(CMAKE_CXX_STANDARD 17)

if (NOT CUDA_INSTALL_DIR)
    set(CUDA_INSTALL_DIR /usr/local/cuda-11.2)
endif()
if (NOT TRT_INSTALL_DIR)
    set(TRT_INSTALL_DIR /home/dskravchenko/TensorRT-8.2.2.1)
endif()
if (NOT CUDNN_INSTALL_DIR)
    set(CUDNN_INSTALL_DIR /home/dskravchenko/cudnn)
endif()

include_directories(
    ${CUDA_INSTALL_DIR}/include
)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


list(APPEND PLUGINS "nvinfer")
list(APPEND PLUGINS "nvonnxparser")
list(APPEND PLUGINS "nvparsers")
list(APPEND PLUGINS "nvinfer_plugin")

foreach(libName ${PLUGINS})
    find_library(${libName}_lib NAMES ${libName} "/usr" PATH_SUFFIXES lib REQUIRED NO_DEFAULT_PATH PATHS ${TRT_INSTALL_DIR})
    list(APPEND PLUGIN_LIBS "${${libName}_lib}")
endforeach()


add_executable(trt_example
    main.cpp
    executor.cpp
    builder.cpp
    options_binder.cpp
    utils.cpp
)

target_link_libraries(trt_example PUBLIC ${CUDA_INSTALL_DIR}/lib64/libcudart.so ${CUDNN_INSTALL_DIR}/lib64/libcudnn.so ${PLUGIN_LIBS})

target_include_directories(trt_example
    PUBLIC ${CUDA_INSTALL_DIR}/include
    PRIVATE contrib/cxxopts/include
    PRIVATE ${TRT_INSTALL_DIR}/include
)
