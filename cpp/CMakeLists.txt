cmake_minimum_required(VERSION 3.10)
project(AdvancedFlooder)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Common source files
set(COMMON_SOURCES
    src/main.cpp
    src/CrossPlatform.h
)

# Check if we're on Windows
if(WIN32)
    # Windows-specific build
    # Add resource compiler for RC files
    enable_language(RC)
    
    # Add Windows-specific sources
    set(PLATFORM_SOURCES
        src/CrossPlatformWindows.cpp
        src/MainWindow.cpp
        src/MessageSender.cpp
        src/ProcessEnumerator.cpp
        resources/resource.rc
    )
    
    # Add executable with resources
    add_executable(${PROJECT_NAME} 
        ${COMMON_SOURCES}
        ${PLATFORM_SOURCES}
    )
    
    # Link Windows libraries
    target_link_libraries(${PROJECT_NAME} 
        user32 
        gdi32 
        kernel32 
        comctl32
        psapi
    )
    
    # Set subsystem for Windows GUI app (can be overridden by command line)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    
    message(STATUS "Building for Windows with GUI and CLI support")
else()
    # Linux/Unix build
    message(STATUS "Building for Linux with full functionality")
    
    # Add Linux-specific sources
    set(PLATFORM_SOURCES
        src/CrossPlatformLinux.cpp
    )
    
    # Add executable
    add_executable(${PROJECT_NAME} 
        ${COMMON_SOURCES}
        ${PLATFORM_SOURCES}
    )
    
    # Find and link X11 libraries
    find_package(X11 REQUIRED)
    if(X11_FOUND)
        target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES})
        target_include_directories(${PROJECT_NAME} PRIVATE ${X11_INCLUDE_DIR})
        
        # Check for XTest extension
        if(X11_XTest_FOUND)
            target_link_libraries(${PROJECT_NAME} ${X11_XTest_LIB})
        else()
            message(WARNING "XTest extension not found. Some functionality may not work.")
        endif()
    else()
        message(FATAL_ERROR "X11 libraries not found. Please install X11 development packages.")
    endif()
    
    # Link pthread for threading support
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME} Threads::Threads)
endif()

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    src
    resources
)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

# Add CLI executable for Windows
if(WIN32)
    add_executable(${PROJECT_NAME}_CLI 
        ${COMMON_SOURCES}
        src/CrossPlatformWindows.cpp
    )
    
    target_link_libraries(${PROJECT_NAME}_CLI 
        user32 
        gdi32 
        kernel32 
        comctl32
        psapi
    )
    
    target_include_directories(${PROJECT_NAME}_CLI PRIVATE 
        src
        resources
    )
    
    # Set as console application
    set_target_properties(${PROJECT_NAME}_CLI PROPERTIES
        WIN32_EXECUTABLE FALSE
    )
endif()