find_package(BISON REQUIRED)
set(BisonOutput ${CMAKE_CURRENT_BINARY_DIR}/MrParser.cpp)
if(BISON_FOUND)
    add_custom_command(
        OUTPUT ${BisonOutput}
        COMMAND ${BISON_EXECUTABLE}
                --output=${BisonOutput}
                --defines=MrParser.h
                ${CMAKE_CURRENT_SOURCE_DIR}/MrParser.y
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/MrParser.y
        COMMENT "Running Bison"
    )
else(BISON_FOUND)
message(FATAL "Could not locate bison")
endif(BISON_FOUND)

find_package(FLEX REQUIRED)
set(FlexOutput ${CMAKE_CURRENT_BINARY_DIR}/MrLexer.cpp)
if(FLEX_FOUND)
    add_custom_command(
        OUTPUT ${FlexOutput}
        COMMAND ${FLEX_EXECUTABLE}
                --outfile=${FlexOutput}
                ${CMAKE_CURRENT_SOURCE_DIR}/MrLexer.l
                DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/MrLexer.l
                        ${CMAKE_CURRENT_BINARY_DIR}/MrParser.h
                COMMENT "Running Flex"
    )
else(FLEX_FOUND)
message(FATAL "Could not locate flex")
endif(FLEX_FOUND)

add_library(libMrMangler itanium.cpp windows.cpp ${FlexOutput} ${BisonOutput})
