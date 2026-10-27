# ---------------------------------------------------------------------------
# packages/restGarfield - optional Garfield++ application package
# Runs Garfield++ tasks (e.g. Magboltz transport tables) from YAML configurations. The REST libraries
# do not depend on Garfield++: the medium files are read by TRestDetectorTransportTable (detector).
# ---------------------------------------------------------------------------

find_package(Garfield REQUIRED HINTS $ENV{GARFIELD_INSTALL} $ENV{GARFIELD_HOME})

file(GLOB_RECURSE RESTGARFIELD_SOURCES src/*.cxx)

add_executable(restGarfield main.cxx ${RESTGARFIELD_SOURCES})
target_include_directories(restGarfield PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(restGarfield PRIVATE RestCore RestTools ${DETECTOR_LIB} Garfield::Garfield
                                           ${ROOT_TARGET_LIBRARIES} yaml-cpp::yaml-cpp)

install(TARGETS restGarfield RUNTIME DESTINATION bin)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples/ DESTINATION examples/restGarfield)
