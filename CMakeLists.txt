cmake_minimum_required(VERSION 3.15)
project(a2a-excise VERSION 1.0.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENTIONS OFF)

# Options
option(BUILD_EXAMPLES "Build example applications" ON)
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Find dependencies
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/third_party)

# Source files
set(A2A_SOURCES
    # Core
    src/core/exception.cpp
    src/core/jsonrpc_request.cpp
    src/core/jsonrpc_response.cpp
    src/core/http_client.cpp
    
    # Models
    src/models/message_part.cpp
    src/models/agent_message.cpp
    src/models/task_status.cpp
    src/models/artifact.cpp
    src/models/agent_task.cpp
    src/models/agent_card.cpp
    src/models/message_send_params.cpp
    
    # Client
    src/client/card_resolver.cpp
    src/client/a2a_client.cpp
    
    # Server
    src/server/memory_task_store.cpp
    src/server/task_manager.cpp
)

# Header files
set(A2A_HEADERS
   # Core
    include/a2a/core/types.hpp
    include/a2a/core/error_code.hpp
    include/a2a/core/exception.hpp
    include/a2a/core/a2a_methods.hpp
    include/a2a/core/jsonrpc_request.hpp
    include/a2a/core/jsonrpc_response.hpp
    include/a2a/core/http_client.hpp
    
    # Models
    include/a2a/models/message_part.hpp
    include/a2a/models/agent_message.hpp
    include/a2a/models/task_status.hpp
    include/a2a/models/artifact.hpp
    include/a2a/models/agent_task.hpp
    include/a2a/models/agent_card.hpp
    include/a2a/models/message_send_params.hpp
    include/a2a/models/a2a_response.hpp
    
    # Client
    include/a2a/client/card_resolver.hpp
    include/a2a/client/a2a_client.hpp
    
    # Server
    include/a2a/server/task_store.hpp
    include/a2a/server/memory_task_store.hpp
    include/a2a/server/task_manager.hpp
)

# Create library
add_library(a2a ${A2A_SOURCES} ${A2A_HEADERS})

# 这里使用了生成器表达式， 用于在构建和安装的不同阶段动态的生成路径或配置
# 构建时：头文件路径是${CMAKE_CURRENT_SOURCE_DIR}/include文件夹
# 安装时：头文件路径是安装到系统中的include目录，相对于CMAKE_INSTALL_PREFIX
# 这种方法可以保证项目在开发和安装后都能找到正确的头文件，避免路径问题
target_link_directories(a2a
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(a2a
    PUBLIC
        CURL::libcurl
        Threads::Threads
)

# Compiler warning
if(MSVC)
    target_link_options(a2a PRIVATE /w4)
else()
    target_link_options(a2a PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Tests
if(BUILD_TESTS)
    enable_testing()
    # TODO: Add tests directory
    # add_subdirectory(tests)
endif()

# Installation
# 这里没有设置CMAKE_INSTALL_PREFIX路径，因此会采用它的默认值，即系统路径usr/local/
# 
install(TARGETS a2a
    EXPORT a2a-targets          # 将目标a2a的安装信息导出到a2a-targets中，用于生成CMake配置文件
    LIBRARY DESTINATION lib     # 如果目标是共享库（SHARED），将其安装到 lib 目录。
    ARCHIVE DESTINATION lib     # 如果目标是静态库（STATIC），将其安装到 lib 目录。
    RUNTIME DESTINATION bin     # 如果目标是可执行文件（EXECUTABLE），将其安装到 bin 目录。
)

install(DIRECTORY include/a2a   # 指定要安装的头文件目录是include/a2a
    DESTINATION include         # 安装到目标系统的include文件夹下，相对于CMAKE_INSTALL_PREFIX
)

install(EXPORT a2a-targets      # 安装导出文件
    FILE a2a-config.cmake       # 生成的配置文件名称
    NAMESPACE a2a::             # 为导出的目标添加命名空间前缀
    DESTINATION lib/cmake/a2a   # 安装到目标系统的lib/cmake/a2a文件夹下，相对于CMAKE_INSTALL_PREFIX
)