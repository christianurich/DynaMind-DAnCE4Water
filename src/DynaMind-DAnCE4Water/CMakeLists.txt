file(GLOB MODULE_CPPS *.cpp)

include_directories(.)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../3rdparty/carve/include)
include_directories(../../3rdparty/carve/include)
include_directories(../../3rdparty/carve/common)
add_library(dance4water SHARED ${MODULE_CPPS})

target_link_libraries(dance4water  carve  carve_misc  ${DYNAMIND_TOOLBOX_LIBRARY}  ${DYNAMIND_EXTENSIONS_LIBRARY} ${DYNAMIND_CORE_LIBRARY} ${QT_LIBRARIES} ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES} )

INSTALL(TARGETS dance4water
        DESTINATION "lib/DynaMind"
        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)


