/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/Basics.h>
#include <Foundation/DataProcessing/Stream/ProcessingStreamGroup.h>
#include <Foundation/DataProcessing/Stream/ProcessingStreamProcessor.h>

// clang-format off
NS_BEGIN_DYNAMIC_REFLECTED_TYPE(nsProcessingStreamProcessor, 1, nsRTTINoAllocator)
NS_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

nsProcessingStreamProcessor::nsProcessingStreamProcessor()

  = default;

nsProcessingStreamProcessor::~nsProcessingStreamProcessor()
{
  m_pStreamGroup = nullptr;
}



NS_STATICLINK_FILE(Foundation, Foundation_DataProcessing_Stream_Implementation_ProcessingStreamProcessor);
