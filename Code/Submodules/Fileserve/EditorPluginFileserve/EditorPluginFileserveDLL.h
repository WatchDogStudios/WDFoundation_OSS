#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Configuration/Plugin.h>

// Configure the DLL Import/Export Define
#if NS_ENABLED(NS_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_EDITORPLUGINFILESERVE_LIB
#    define NS_EDITORPLUGINFILESERVE_DLL NS_DECL_EXPORT
#  else
#    define NS_EDITORPLUGINFILESERVE_DLL NS_DECL_IMPORT
#  endif
#else
#  define NS_EDITORPLUGINFILESERVE_DLL
#endif