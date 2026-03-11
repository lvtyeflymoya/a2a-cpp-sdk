# Echo Agent
add_executable(echo_agent
    echo_agent/main.cpp
)

target_link_libraries(echo_agent
    PRIVATE a2a
)

# Multi-Agent Demo
add_subdirectory(multi_agent_demo)

# Installation
install(TARGETS echo_agent
    RUNTIME DESTINATION bin/examples    
)