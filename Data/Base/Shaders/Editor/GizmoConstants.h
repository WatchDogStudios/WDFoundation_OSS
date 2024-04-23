/*
 *   Copyright (c) 2023 WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include "../Common/Platforms.h"
#include "../Common/ConstantBufferMacros.h"

CONSTANT_BUFFER(nsGizmoConstants, 2)
{
	MAT4(ObjectToWorldMatrix);
	MAT4(WorldToObjectMatrix);
	COLOR4F(GizmoColor);
	FLOAT1(GizmoScale);
	INT1(GameObjectID);
};
