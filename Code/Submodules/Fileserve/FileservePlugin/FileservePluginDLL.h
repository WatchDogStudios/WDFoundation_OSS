#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Configuration/Plugin.h>

// Configure the DLL Import/Export Define
#if NS_ENABLED(NS_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_FILESERVEPLUGIN_LIB
#    define NS_FILESERVEPLUGIN_DLL NS_DECL_EXPORT
#  else
#    define NS_FILESERVEPLUGIN_DLL NS_DECL_IMPORT
#  endif
#else
#  define NS_FILESERVEPLUGIN_DLL
#endif
