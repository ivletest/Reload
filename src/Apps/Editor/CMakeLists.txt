set(EDITOR_PROJECT_NAME ReloadEditor)

add_recursive( ${CMAKE_CURRENT_LIST_DIR}/src SOURCES )
add_recursive( ${CMAKE_CURRENT_LIST_DIR}/include HEADERS )

set(IMGUI_IMPL_SOURCES 
    ${CMAKE_SOURCE_DIR}/lib/imgui/backends/imgui_impl_sdl.cpp
    ${CMAKE_SOURCE_DIR}/lib/imgui/backends/imgui_impl_opengl3.cpp)
set(IMGUI_IMPL_HEADERS 
    ${CMAKE_SOURCE_DIR}/lib/imgui/backends/imgui_impl_sdl.h
    ${CMAKE_SOURCE_DIR}/lib/imgui/backends/imgui_impl_opengl3.h)

# set(EXECUTABLE_OUTPUT_PATH "${APPS_BUILD_DIRECTORY}/Editor/${CMAKE_BUILD_TYPE}" )
add_executable(${EDITOR_PROJECT_NAME} WIN32 MACOSX_BUNDLE 
    ${SOURCES} 
    ${HEADERS} 
    ${RESOURCES}
    ${IMGUI_IMPL_SOURCES}
    ${IMGUI_IMPL_HEADERS}
)

target_include_directories(${CORE_PROJECT_NAME}
    PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib/imgui/backends
)

target_link_libraries(${EDITOR_PROJECT_NAME} PRIVATE
    Corrade::Main
    Corrade::Utility
    Corrade::Containers
    Magnum::Application
    Magnum::GL
    Magnum::Magnum
    Magnum::MeshTools
    Magnum::SceneGraph
    Magnum::Shaders
    Magnum::Trade
    MagnumIntegration::ImGui
    ReloadCore)

add_dependencies(${EDITOR_PROJECT_NAME}
    Magnum::AnyImageImporter
    Magnum::AnySceneImporter
    Magnum::ObjImporter
    MagnumPlugins::StbImageImporter
    MagnumPlugins::TinyGltfImporter
    MagnumPlugins::AssimpImporter)

# target_include_directories(${EDITOR_PROJECT_NAME} PRIVATE 
#     $<BUILD_INTERFACE: ${CMAKE_SOURCE_DIR}/core/include>)