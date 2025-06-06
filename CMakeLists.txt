CMAKE_MINIMUM_REQUIRED(VERSION 3.23)

PROJECT(LiteFXSample LANGUAGES CXX)

# Setup build artifact directory.
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "$<1:${CMAKE_BINARY_DIR}/binaries/>")
 
SET(CMAKE_CXX_STANDARD 23)
FIND_PACKAGE(LiteFX CONFIG REQUIRED)
FIND_PACKAGE(glfw3 CONFIG REQUIRED)
FIND_PACKAGE(CLI11 CONFIG REQUIRED)
 
ADD_EXECUTABLE(${PROJECT_NAME} "src/main.h" "src/main.cpp" "src/sample.h" "src/sample.cpp")
TARGET_LINK_LIBRARIES(${PROJECT_NAME} PRIVATE LiteFX.Backends.Vulkan LiteFX.Backends.DirectX12 glfw CLI11::CLI11)

ADD_SHADER_MODULE(${PROJECT_NAME}.Vk.Shaders.VS SOURCE "shaders/basic_vs.hlsl" LANGUAGE HLSL TYPE VERTEX   COMPILE_AS SPIRV SHADER_MODEL 6_5 COMPILER DXC)
ADD_SHADER_MODULE(${PROJECT_NAME}.Vk.Shaders.FS SOURCE "shaders/basic_fs.hlsl" LANGUAGE HLSL TYPE FRAGMENT COMPILE_AS SPIRV SHADER_MODEL 6_5 COMPILER DXC)

TARGET_LINK_SHADERS(${PROJECT_NAME} 
    INSTALL_DESTINATION "${CMAKE_INSTALL_BINARY_DIR}/${SHADER_DEFAULT_SUBDIR}"
    SHADERS ${PROJECT_NAME}.Vk.Shaders.VS ${PROJECT_NAME}.Vk.Shaders.FS
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} PRIVATE LiteFX.Backends.DirectX12)

ADD_SHADER_MODULE(${PROJECT_NAME}.Dx.Shaders.VS SOURCE "shaders/basic_vs.hlsl" LANGUAGE HLSL TYPE VERTEX   COMPILE_AS DXIL SHADER_MODEL 6_5 COMPILER DXC)
ADD_SHADER_MODULE(${PROJECT_NAME}.Dx.Shaders.PS SOURCE "shaders/basic_fs.hlsl" LANGUAGE HLSL TYPE PIXEL    COMPILE_AS DXIL SHADER_MODEL 6_5 COMPILER DXC)

TARGET_LINK_SHADERS(${PROJECT_NAME} 
    INSTALL_DESTINATION "${CMAKE_INSTALL_BINARY_DIR}/${SHADER_DEFAULT_SUBDIR}"
    SHADERS ${PROJECT_NAME}.Dx.Shaders.VS ${PROJECT_NAME}.Dx.Shaders.PS
)