cmake_minimum_required(VERSION 3.11)

project(course_registration LANGUAGES CXX VERSION 1.0.0)

# 设置MSVC运行时库设置
if(MSVC)
    # 对于Debug配置使用/MDd，对于Release配置使用/MD
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif()

# 设置FTXUI库的路径
set(FTXUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ftxui-6.0.2-win64")
list(APPEND CMAKE_PREFIX_PATH ${FTXUI_DIR})

# 查找FTXUI包
find_package(ftxui REQUIRED)

# 添加可执行文件
add_executable(course_registration main.cpp)

# 链接FTXUI库
target_link_libraries(course_registration
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
)

# 设置C++标准
set_target_properties(course_registration PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON)

# 设置调试信息
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG") 