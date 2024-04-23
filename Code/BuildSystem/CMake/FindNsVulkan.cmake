# find the folder into which the Vulkan SDK has been installed

# early out, if this target has been created before
if((TARGET NsVulkan::Loader) AND(TARGET NsVulkan::DXC))
	return()
endif()

set(NS_VULKAN_DIR $ENV{VULKAN_SDK} CACHE PATH "Directory of the Vulkan SDK")

ns_pull_compiler_and_architecture_vars()
ns_pull_config_vars()

get_property(NS_SUBMODULE_PREFIX_PATH GLOBAL PROPERTY NS_SUBMODULE_PREFIX_PATH)

if(NS_CMAKE_PLATFORM_WINDOWS_DESKTOP AND NS_CMAKE_ARCHITECTURE_64BIT)
	if((NS_VULKAN_DIR STREQUAL "NS_VULKAN_DIR-NOTFOUND") OR(NS_VULKAN_DIR STREQUAL ""))
		# set(CMAKE_FIND_DEBUG_MODE TRUE)
		unset(NS_VULKAN_DIR CACHE)
		unset(NsVulkan_DIR CACHE)
		find_path(NS_VULKAN_DIR Config/vk_layer_settings.txt
			PATHS
			${NS_VULKAN_DIR}
			$ENV{VULKAN_SDK}
			REQUIRED
		)

		# set(CMAKE_FIND_DEBUG_MODE FALSE)
	endif()
elseif(NS_CMAKE_PLATFORM_LINUX AND NS_CMAKE_ARCHITECTURE_64BIT)
	if((NS_VULKAN_DIR STREQUAL "NS_VULKAN_DIR-NOTFOUND") OR(NS_VULKAN_DIR STREQUAL ""))
		# set(CMAKE_FIND_DEBUG_MODE TRUE)
		unset(NS_VULKAN_DIR CACHE)
		unset(NsVulkan_DIR CACHE)
		find_path(NS_VULKAN_DIR config/vk_layer_settings.txt
			PATHS
			${NS_VULKAN_DIR}
			$ENV{VULKAN_SDK}
		)

		if(NS_CMAKE_ARCHITECTURE_X86)
			if((NS_VULKAN_DIR STREQUAL "NS_VULKAN_DIR-NOTFOUND") OR (NS_VULKAN_DIR STREQUAL ""))
				ns_download_and_extract("${NS_CONFIG_VULKAN_SDK_LINUXX64_URL}" "${CMAKE_BINARY_DIR}/vulkan-sdk" "vulkan-sdk-${NS_CONFIG_VULKAN_SDK_LINUXX64_VERSION}")
				set(NS_VULKAN_DIR "${CMAKE_BINARY_DIR}/vulkan-sdk/${NS_CONFIG_VULKAN_SDK_LINUXX64_VERSION}" CACHE PATH "Directory of the Vulkan SDK" FORCE)

				find_path(NS_VULKAN_DIR config/vk_layer_settings.txt
					PATHS
					${NS_VULKAN_DIR}
					$ENV{VULKAN_SDK}
				)
			endif()
		endif()

		if((NS_VULKAN_DIR STREQUAL "NS_VULKAN_DIR-NOTFOUND") OR (NS_VULKAN_DIR STREQUAL ""))
			message(FATAL_ERROR "Failed to find vulkan SDK. Ns requires the vulkan sdk ${NS_CONFIG_VULKAN_SDK_LINUXX64_VERSION}. Please set the environment variable VULKAN_SDK to the vulkan sdk location.")
		endif()

		# set(CMAKE_FIND_DEBUG_MODE FALSE)
	endif()
else()
	message(FATAL_ERROR "TODO: Vulkan is not yet supported on this platform and/or architecture.")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NsVulkan DEFAULT_MSG NS_VULKAN_DIR)

if(NSVULKAN_FOUND)
	if(NS_CMAKE_PLATFORM_WINDOWS_DESKTOP AND NS_CMAKE_ARCHITECTURE_64BIT)
		add_library(NsVulkan::Loader STATIC IMPORTED)
		set_target_properties(NsVulkan::Loader PROPERTIES IMPORTED_LOCATION "${NS_VULKAN_DIR}/Lib/vulkan-1.lib")
		set_target_properties(NsVulkan::Loader PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${NS_VULKAN_DIR}/Include")

		add_library(NsVulkan::DXC SHARED IMPORTED)
		set_target_properties(NsVulkan::DXC PROPERTIES IMPORTED_LOCATION "${NS_VULKAN_DIR}/Bin/dxcompiler.dll")
		set_target_properties(NsVulkan::DXC PROPERTIES IMPORTED_IMPLIB "${NS_VULKAN_DIR}/Lib/dxcompiler.lib")
		set_target_properties(NsVulkan::DXC PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${NS_VULKAN_DIR}/Include")

	elseif(NS_CMAKE_PLATFORM_LINUX AND NS_CMAKE_ARCHITECTURE_64BIT)
		add_library(NsVulkan::Loader SHARED IMPORTED)
		set_target_properties(NsVulkan::Loader PROPERTIES IMPORTED_LOCATION "${NS_VULKAN_DIR}/x86_64/lib/libvulkan.so.1.3.216")
		set_target_properties(NsVulkan::Loader PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${NS_VULKAN_DIR}/x86_64/include")

		add_library(NsVulkan::DXC SHARED IMPORTED)
		set_target_properties(NsVulkan::DXC PROPERTIES IMPORTED_LOCATION "${NS_VULKAN_DIR}/x86_64/lib/libdxcompiler.so.3.7")
		set_target_properties(NsVulkan::DXC PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${NS_VULKAN_DIR}/x86_64/include")
	else()
		message(FATAL_ERROR "TODO: Vulkan is not yet supported on this platform and/or architecture.")
	endif()
endif()
