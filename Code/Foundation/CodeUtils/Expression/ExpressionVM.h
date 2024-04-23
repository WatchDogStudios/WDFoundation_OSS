/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/CodeUtils/Expression/ExpressionByteCode.h>
#include <Foundation/Types/UniquePtr.h>

class NS_FOUNDATION_DLL nsExpressionVM
{
public:
  nsExpressionVM();
  ~nsExpressionVM();

  void RegisterFunction(const nsExpressionFunction& func);
  void UnregisterFunction(const nsExpressionFunction& func);

  nsResult Execute(const nsExpressionByteCode& byteCode, nsArrayPtr<const nsProcessingStream> inputs, nsArrayPtr<nsProcessingStream> outputs, nsUInt32 uiNumInstances, const nsExpression::GlobalData& globalData = nsExpression::GlobalData());

private:
  void RegisterDefaultFunctions();

  static nsResult ScalarizeStreams(nsArrayPtr<const nsProcessingStream> streams, nsDynamicArray<nsProcessingStream>& out_ScalarizedStreams);
  static nsResult MapStreams(nsArrayPtr<const nsExpression::StreamDesc> streamDescs, nsArrayPtr<nsProcessingStream> streams, nsStringView sStreamType, nsUInt32 uiNumInstances, nsDynamicArray<nsProcessingStream*>& out_MappedStreams);
  nsResult MapFunctions(nsArrayPtr<const nsExpression::FunctionDesc> functionDescs, const nsExpression::GlobalData& globalData);

  nsDynamicArray<nsExpression::Register, nsAlignedAllocatorWrapper> m_Registers;

  nsDynamicArray<nsProcessingStream> m_ScalarizedInputs;
  nsDynamicArray<nsProcessingStream> m_ScalarizedOutputs;

  nsDynamicArray<nsProcessingStream*> m_MappedInputs;
  nsDynamicArray<nsProcessingStream*> m_MappedOutputs;
  nsDynamicArray<const nsExpressionFunction*> m_MappedFunctions;

  nsDynamicArray<nsExpressionFunction> m_Functions;
  nsHashTable<nsHashedString, nsUInt32> m_FunctionNamesToIndex;
};
