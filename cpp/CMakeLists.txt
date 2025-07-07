cmake_minimum_required(VERSION 3.10)
project(AdvancedFlooder)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check if we're on Windows
if(WIN32)
    # Windows-specific build
    # Add resource compiler for RC files
    enable_language(RC)
    
    # Add executable with resources
    add_executable(${PROJECT_NAME} 
        src/main.cpp
        src/MainWindow.cpp
        src/MessageSender.cpp
        src/ProcessEnumerator.cpp
        resources/resource.rc
    )
    
    # Link Windows libraries
    target_link_libraries(${PROJECT_NAME} 
        user32 
        gdi32 
        kernel32 
        comctl32
        psapi
    )
    
    # Set subsystem for Windows GUI app
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
else()
    # Non-Windows build (for compilation testing only)
    message(WARNING "This application is designed for Windows only. Building for compilation testing.")
    
    # Add executable without resources
    add_executable(${PROJECT_NAME} 
        src/main.cpp
        src/MainWindow.cpp
        src/MessageSender.cpp
        src/ProcessEnumerator.cpp
    )
    
    # Add dummy implementations for non-Windows
    target_compile_definitions(${PROJECT_NAME} PRIVATE COMPILE_TEST_ONLY)
endif()

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    src
    resources
)