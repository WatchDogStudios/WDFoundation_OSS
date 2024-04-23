/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/IO/Stream.h>
#include <Foundation/IO/StringDeduplicationContext.h>
#include <Foundation/Strings/String.h>

nsStreamReader::nsStreamReader() = default;
nsStreamReader::~nsStreamReader() = default;

nsResult nsStreamReader::ReadString(nsStringBuilder& ref_sBuilder)
{
  if (auto context = nsStringDeduplicationReadContext::GetContext())
  {
    ref_sBuilder = context->DeserializeString(*this);
  }
  else
  {
    nsUInt32 uiCount = 0;
    NS_SUCCEED_OR_RETURN(ReadDWordValue(&uiCount));

    if (uiCount > 0)
    {
      // We access the string builder directly here to
      // read the string efficiently with one allocation
      ref_sBuilder.m_Data.Reserve(uiCount + 1);
      ref_sBuilder.m_Data.SetCountUninitialized(uiCount);
      ReadBytes(ref_sBuilder.m_Data.GetData(), uiCount);
      ref_sBuilder.m_uiCharacterCount = uiCount;
      ref_sBuilder.AppendTerminator();
    }
    else
    {
      ref_sBuilder.Clear();
    }
  }

  return NS_SUCCESS;
}

nsResult nsStreamReader::ReadString(nsString& ref_sString)
{
  nsStringBuilder tmp;
  const nsResult res = ReadString(tmp);
  ref_sString = tmp;

  return res;
}

nsStreamWriter::nsStreamWriter() = default;
nsStreamWriter::~nsStreamWriter() = default;

nsResult nsStreamWriter::WriteString(const nsStringView sStringView)
{
  const nsUInt32 uiCount = sStringView.GetElementCount();

  if (auto context = nsStringDeduplicationWriteContext::GetContext())
  {
    context->SerializeString(sStringView, *this);
  }
  else
  {
    NS_SUCCEED_OR_RETURN(WriteDWordValue(&uiCount));
    if (uiCount > 0)
    {
      NS_SUCCEED_OR_RETURN(WriteBytes(sStringView.GetStartPointer(), uiCount));
    }
  }

  return NS_SUCCESS;
}


NS_STATICLINK_FILE(Foundation, Foundation_IO_Implementation_Stream);
