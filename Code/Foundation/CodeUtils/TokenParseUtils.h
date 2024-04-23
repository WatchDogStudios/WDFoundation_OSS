/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/CodeUtils/Tokenizer.h>
#include <Foundation/Containers/HybridArray.h>

namespace nsTokenParseUtils
{
  using TokenStream = nsHybridArray<const nsToken*, 32>;

  NS_FOUNDATION_DLL void SkipWhitespace(const TokenStream& tokens, nsUInt32& ref_uiCurToken);
  NS_FOUNDATION_DLL void SkipWhitespaceAndNewline(const TokenStream& tokens, nsUInt32& ref_uiCurToken);
  NS_FOUNDATION_DLL bool IsEndOfLine(const TokenStream& tokens, nsUInt32 uiCurToken, bool bIgnoreWhitespace);
  NS_FOUNDATION_DLL void CopyRelevantTokens(const TokenStream& source, nsUInt32 uiFirstSourceToken, TokenStream& ref_destination, bool bPreserveNewLines);

  NS_FOUNDATION_DLL bool Accept(const TokenStream& tokens, nsUInt32& ref_uiCurToken, nsStringView sToken, nsUInt32* pAccepted = nullptr);
  NS_FOUNDATION_DLL bool Accept(const TokenStream& tokens, nsUInt32& ref_uiCurToken, nsTokenType::Enum type, nsUInt32* pAccepted = nullptr);
  NS_FOUNDATION_DLL bool Accept(const TokenStream& tokens, nsUInt32& ref_uiCurToken, nsStringView sToken1, nsStringView sToken2, nsUInt32* pAccepted = nullptr);
  NS_FOUNDATION_DLL bool AcceptUnless(const TokenStream& tokens, nsUInt32& ref_uiCurToken, nsStringView sToken1, nsStringView sToken2, nsUInt32* pAccepted = nullptr);

  NS_FOUNDATION_DLL void CombineTokensToString(const TokenStream& tokens, nsUInt32 uiCurToken, nsStringBuilder& ref_sResult, bool bKeepComments = true, bool bRemoveRedundantWhitespace = false, bool bInsertLine = false);
  NS_FOUNDATION_DLL void CombineRelevantTokensToString(const TokenStream& tokens, nsUInt32 uiCurToken, nsStringBuilder& ref_sResult);
  NS_FOUNDATION_DLL void CreateCleanTokenStream(const TokenStream& tokens, nsUInt32 uiCurToken, TokenStream& ref_destination, bool bKeepComments);
} // namespace nsTokenParseUtils
