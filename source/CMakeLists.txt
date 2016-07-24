add_library(libMrMangler itanium.cpp windows.cpp)

add_executable(MrMangler main.cpp)
target_link_libraries(MrMangler LINK_PUBLIC libMrMangler)
