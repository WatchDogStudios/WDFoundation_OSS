# #####################################
# ## NuGet support
# #####################################

# #####################################
# ## ns_nuget_init()
# #####################################

function(ns_nuget_init)
	find_program(NUGET nuget
		HINTS ${CMAKE_BINARY_DIR})

	if(NOT NUGET)
		message(STATUS "Downloading Nuget...")
		file(DOWNLOAD
			https://dist.nuget.org/win-x86-commandline/latest/nuget.exe
			${CMAKE_BINARY_DIR}/nuget.exe
			SHOW_PROGRESS
		)
		find_program(NUGET nuget
			HINTS ${CMAKE_BINARY_DIR})
	endif()

	mark_as_advanced(FORCE NUGET)
endfunction()

function(ns_nuget_get_packages PROJECT_NAME INSTALL_DIR)
	ns_nuget_init()
	add_custom_command(TARGET ${PROJECT_NAME}
		PRE_BUILD
		COMMAND ${CMAKE_SOURCE_DIR}/nuget.exe restore ${CMAKE_CURRENT_SOURCE_DIR}/packages.config -PackagesDirectory ${INSTALL_DIR}
	)
endfunction()