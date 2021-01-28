# Find required packages
find_package(MagnumExtras REQUIRED)
find_package(Corrade REQUIRED Main Utility Containers)
find_package(MagnumIntegration REQUIRED ImGui)
find_package(Magnum REQUIRED
    GL
    MeshTools
    Shaders
    SceneGraph
	Trade
)

# If tergeting Android
if(CORRADE_TARGET_ANDROID)
	find_package(Magnum REQUIRED AndroidApplication)
	add_definitions(-DANDROID_APPLICATION)
# If tergeting WebAssembly
elseif(CORRADE_TARGET_EMSCRIPTEN)
	find_package(Magnum REQUIRED EmscriptenApplication)
	add_definitions(-DEMSCRIPTEN_APPLICATION)
# If targeting PC or console
else()
	find_package(Magnum REQUIRED Sdl2Application)
	add_definitions(-DSDL2_APPLICATION)
endif()

add_definitions(-DOPENGL)

set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

# Setup Core project
set(CORE_PROJECT_NAME ReloadCore)
add_library(${CORE_PROJECT_NAME} OBJECT)
include(${CMAKE_CURRENT_LIST_DIR}/Core/CMakeLists.txt)

# Setup applications
include(${CMAKE_CURRENT_LIST_DIR}/Editor/CMakeLists.txt)
if(RELOAD_EDITOR)
endif()