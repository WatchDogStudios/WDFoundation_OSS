/*
 *   Copyright (c) 2023 WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#if (BLEND_MODE == BLEND_MODE_OPAQUE)
RenderDataCategory = LitOpaque
#elif (BLEND_MODE == BLEND_MODE_MASKED)
RenderDataCategory = LitMasked
#else
RenderDataCategory = LitTransparent
#endif
