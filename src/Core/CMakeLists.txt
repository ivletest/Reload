add_recursive( ${CMAKE_CURRENT_LIST_DIR}/src SOURCES )
add_recursive( ${CMAKE_CURRENT_LIST_DIR}/include HEADERS )

target_sources(${CORE_PROJECT_NAME}
    PRIVATE ${SOURCES}
    PUBLIC ${HEADERS}
)

target_include_directories(${CORE_PROJECT_NAME}
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(${CORE_PROJECT_NAME} PRIVATE
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

add_dependencies(${CORE_PROJECT_NAME}
    Magnum::AnyImageImporter
    Magnum::AnySceneImporter
    Magnum::ObjImporter
    MagnumPlugins::StbImageImporter
    MagnumPlugins::TinyGltfImporter
    MagnumPlugins::AssimpImporter
)