set_source_files_properties(
    tasksys.cc
    PROPERTIES
    COMPILE_FLAGS -Wno-unused-function
)

add_library(common_objs
    OBJECT
    tasksys.cc
)
target_link_libraries(common_objs PUBLIC Threads::Threads)