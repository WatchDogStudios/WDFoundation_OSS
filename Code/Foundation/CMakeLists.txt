ns_cmake_init()

# Get the name of this folder as the project name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

ns_create_target(LIBRARY ${PROJECT_NAME} NO_NS_PREFIX)

if(NS_CMAKE_PLATFORM_WINDOWS)
  target_link_libraries(${PROJECT_NAME}
    PRIVATE

    Rpcrt4.lib
  )
endif()

ns_enable_strict_warnings(${PROJECT_NAME})

if(NS_CMAKE_PLATFORM_LINUX)
  CHECK_INCLUDE_FILE_CXX("uuid/uuid.h" UUID_HEADER)

  if(NOT UUID_HEADER)
    message(FATAL_ERROR "Missing uuid/uuid.h. Have you installed the uuid-dev package?")
  endif()

  target_link_libraries(${PROJECT_NAME}
    PRIVATE

    uuid
    dl # dlopen, dlclose, etc
  )
endif()

if(CURRENT_OSX_VERSION)
  find_library(CORESERVICES_LIBRARY CoreServices)
  find_library(COREFOUNDATION_LIBRARY CoreFoundation)

  mark_as_advanced(FORCE CORESERVICES_LIBRARY COREFOUNDATION_LIBRARY)

  target_link_libraries(${PROJECT_NAME}
    PRIVATE

    ${CORESERVICES_LIBRARY}
    ${COREFOUNDATION_LIBRARY}
  )
endif()

if(NS_3RDPARTY_ENET_SUPPORT)
  target_link_libraries(${PROJECT_NAME} PUBLIC enet)
endif()

if(NS_3RDPARTY_ZSTD_SUPPORT)
  target_link_libraries(${PROJECT_NAME} PUBLIC zstd)
endif()

if(NS_3RDPARTY_GLFW_SUPPORT)
  target_compile_definitions(${PROJECT_NAME} PUBLIC BUILDSYSTEM_ENABLE_GLFW_SUPPORT)
  target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
endif()

if(NS_3RDPARY_MIMALLOC_SUPPORT)
  # Add MIMalloc
  target_link_libraries(${PROJECT_NAME} PRIVATE mimalloc)
endif()

set_target_properties(${PROJECT_NAME}
  PROPERTIES
  OUTPUT_NAME "wdcorestdlib"
)
