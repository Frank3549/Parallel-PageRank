# Add current directory to include search path (so tests can find headers)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Modify this file as you need for your project (modeled on the most relevant assignment), e.g.
# uncomment the following and add the relevant files for your project
#add_executable(project-main
#  main.cc
#)

# Add current directory to include search path
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Find OpenMP
find_package(OpenMP REQUIRED)

# Create pagerank executable from pagerank.cc
add_executable(pagerank pagerank.cc)
add_executable(pagerankSerial pagerankSerial.cc)
add_executable(syntheticGraph syntheticGraph.cc)
# Link OpenMP 
target_link_libraries(pagerank PUBLIC OpenMP::OpenMP_CXX Threads::Threads)
