/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/IO/DeduplicationReadContext.h>
#include <Foundation/IO/DeduplicationWriteContext.h>

NS_IMPLEMENT_SERIALIZATION_CONTEXT(nsDeduplicationReadContext);

nsDeduplicationReadContext::nsDeduplicationReadContext() = default;
nsDeduplicationReadContext::~nsDeduplicationReadContext() = default;

//////////////////////////////////////////////////////////////////////////

NS_IMPLEMENT_SERIALIZATION_CONTEXT(nsDeduplicationWriteContext);

nsDeduplicationWriteContext::nsDeduplicationWriteContext() = default;
nsDeduplicationWriteContext::~nsDeduplicationWriteContext() = default;



NS_STATICLINK_FILE(Foundation, Foundation_IO_Implementation_DeduplicationContext);
