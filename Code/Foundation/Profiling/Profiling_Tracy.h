#include <Foundation/Basics.h>
#include <Foundation/Strings/HashedString.h>
#if NS_ENABLED(NS_USE_PROFILING) || defined(NS_DOCS)
#  if defined(TRACY_ENABLE)
#    include <tracy/tracy/Tracy.hpp>

NS_ALWAYS_INLINE nsUInt32 ___tracyGetStringLength(const char* szString)
{
  return nsStringUtils::GetStringElementCount(szString);
}

NS_ALWAYS_INLINE nsUInt32 ___tracyGetStringLength(nsStringView szString)
{
  return szString.GetElementCount();
}

NS_ALWAYS_INLINE nsUInt32 ___tracyGetStringLength(const nsString& szString)
{
  return szString.GetElementCount();
}
NS_ALWAYS_INLINE nsUInt32 ___tracyGetStringLength(const nsStringBuilder& szString)
{
  return szString.GetElementCount();
}
NS_ALWAYS_INLINE nsUInt32 ___tracyGetStringLength(const nsHashedString& szString)
{
  return szString.GetView().GetElementCount();
}
NS_ALWAYS_INLINE const char* __convertnsStringToConstChar(const nsString& szString)
{
  return szString.GetData();
}
NS_ALWAYS_INLINE const char* __convertnsStringToConstChar(const nsStringBuilder& szString)
{
  return szString.GetData();
}
NS_ALWAYS_INLINE const char* __convertnsStringToConstChar(const nsHashedString& szString)
{
  return szString.GetData();
}
NS_ALWAYS_INLINE const char* __convertnsStringToConstChar(const nsStringView& szString)
{
  nsStringBuilder temp_str;
  szString.GetData(temp_str);
  return temp_str.GetData();
}
/// Let the accepted types pass through.
NS_ALWAYS_INLINE const char* __convertnsStringToConstChar(const char* szString)
{
  return szString;
}
#    define TRACY_PROFILE_SCOPE_DYNAMIC(szScopeName) \
      ZoneScoped;                                    \
      ZoneName(__convertnsStringToConstChar(szScopeName), ___tracyGetStringLength(szScopeName))

/// \brief Profiles the current scope using the given name.
///
/// It is allowed to nest NS_PROFILE_SCOPE, also with NS_PROFILE_LIST_SCOPE. However NS_PROFILE_SCOPE should start and end within the same list scope
/// section.
///
/// \note The name string must not be destroyed before the current scope ends.
///
/// \sa nsProfilingScope
/// \sa NS_PROFILE_LIST_SCOPE
#    define NS_PROFILE_SCOPE(szScopeName)                                                                                 \
      nsProfilingScope NS_CONCAT(_nsProfilingScope, NS_SOURCE_LINE)(szScopeName, NS_SOURCE_FUNCTION, nsTime::MakeZero()); \
      TRACY_PROFILE_SCOPE_DYNAMIC(szScopeName)

/// \brief Same as NS_PROFILE_SCOPE but if the scope takes longer than 'Timeout', the nsProfilingSystem's timeout callback is executed.
///
/// This can be used to log an error or save a callstack, etc. when a scope exceeds an expected amount of time.
///
/// \sa nsProfilingSystem::SetScopeTimeoutCallback()
#    define NS_PROFILE_SCOPE_WITH_TIMEOUT(szScopeName, Timeout) \
      TRACY_PROFILE_SCOPE_DYNAMIC(szScopeName);
/// \brief Profiles the current scope using the given name as the overall list scope name and the section name for the first section in the list.
///
/// Use NS_PROFILE_LIST_NEXT_SECTION to start a new section in the list scope.
///
/// It is allowed to nest NS_PROFILE_SCOPE, also with NS_PROFILE_LIST_SCOPE. However NS_PROFILE_SCOPE should start and end within the same list scope
/// section.
///
/// \note The name string must not be destroyed before the current scope ends.
///
/// \sa nsProfilingListScope
/// \sa NS_PROFILE_LIST_NEXT_SECTION
#    define NS_PROFILE_LIST_SCOPE(szListName, szFirstSectionName) \
      TRACY_PROFILE_SCOPE_DYNAMIC(szScopeName);

/// \brief Starts a new section in a NS_PROFILE_LIST_SCOPE
///
/// \sa nsProfilingListScope
/// \sa NS_PROFILE_LIST_SCOPE
#    define NS_PROFILE_LIST_NEXT_SECTION(szNextSectionName) nsProfilingListScope::StartNextSection(szNextSectionName)

#    define NS_PROFILER_END_FRAME FrameMark;
#  endif
#endif
