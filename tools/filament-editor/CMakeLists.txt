cmake_minimum_required(VERSION 3.19)
project(filament-editor)

set(TARGET filament-editor)

# ==================================================================================================
# Sources and headers
# ==================================================================================================
set(PUBLIC_HDRS
    include/filament-editor/EditorContext.h
    include/filament-editor/core/EditorTypes.h
    include/filament-editor/core/SelectionManager.h
    include/filament-editor/core/CommandHistory.h
    include/filament-editor/ui/IPanel.h
)

set(SRCS
    src/EditorMain.cpp
    src/EditorContext.cpp
    src/core/SelectionManager.cpp
    src/core/CommandHistory.cpp
)

# ==================================================================================================
# Target definitions
# ==================================================================================================
add_executable(${TARGET} ${PUBLIC_HDRS} ${SRCS})

target_include_directories(${TARGET} PRIVATE include)

target_link_libraries(${TARGET} PRIVATE filamentapp)

set_target_properties(${TARGET} PROPERTIES FOLDER Tools)

# ==================================================================================================
# Installation
# ==================================================================================================
install(TARGETS ${TARGET} RUNTIME DESTINATION bin)
