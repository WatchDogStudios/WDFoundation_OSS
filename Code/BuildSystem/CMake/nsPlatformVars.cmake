# Cmake variables which are platform dependent

# #####################################
# ## General settings
# #####################################
if(NS_CMAKE_PLATFORM_WINDOWS OR NS_CMAKE_PLATFORM_LINUX)
	set(NS_COMPILE_ENGINE_AS_DLL ON CACHE BOOL "Whether to compile the code as a shared libraries (DLL).")
	mark_as_advanced(FORCE NS_COMPILE_ENGINE_AS_DLL)
else()
	unset(NS_COMPILE_ENGINE_AS_DLL CACHE)
endif()

# #####################################
# ## Experimental Editor support on Linux
# #####################################
if(NS_CMAKE_PLATFORM_LINUX)
	set (NS_EXPERIMENTAL_EDITOR_ON_LINUX OFF CACHE BOOL "Wether or not to build the editor on linux")
endif()

set (NS_EXPERIMENTAL_D3D12_RENDERER OFF CACHE BOOL "Wether or not to build the Experimental d3d12 renderer")