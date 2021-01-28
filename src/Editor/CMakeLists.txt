set(EDITOR_PROJECT_NAME ReloadEditor)

add_recursive( ${CMAKE_CURRENT_LIST_DIR}/src SOURCES )
add_recursive( ${CMAKE_CURRENT_LIST_DIR}/include HEADERS )

corrade_add_resource(RESOURCES ${CMAKE_CURRENT_LIST_DIR}/resources.conf)

add_executable(${EDITOR_PROJECT_NAME} WIN32 MACOSX_BUNDLE 
    ${SOURCES} 
    ${HEADERS} 
    ${RESOURCES}
)

target_include_directories(${CORE_PROJECT_NAME}
    PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(${EDITOR_PROJECT_NAME} PRIVATE 
    ReloadCore
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
    MagnumExtras::Ui
    MagnumIntegration::ImGui)

    add_dependencies(${EDITOR_PROJECT_NAME}
    Magnum::AnyImageImporter
    Magnum::AnySceneImporter
    Magnum::ObjImporter
    Magnum::TgaImporter
    MagnumPlugins::StbImageImporter
    MagnumPlugins::TinyGltfImporter
    MagnumPlugins::AssimpImporter
)
