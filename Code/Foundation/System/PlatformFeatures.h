/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#undef NS_SUPPORTS_GLFW
#ifdef BUILDSYSTEM_ENABLE_GLFW_SUPPORT
#  define NS_SUPPORTS_GLFW NS_ON
#else
#  define NS_SUPPORTS_GLFW NS_OFF
#endif
