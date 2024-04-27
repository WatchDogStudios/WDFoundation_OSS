#include <FileservePlugin/FileservePluginPCH.h>

#include <FileservePlugin/Client/FileserveDataDir.h>

// clang-format off
NS_BEGIN_SUBSYSTEM_DECLARATION(FileservePlugin, FileservePluginMain)

  BEGIN_SUBSYSTEM_DEPENDENCIES
    "Foundation"
  END_SUBSYSTEM_DEPENDENCIES

  ON_CORESYSTEMS_STARTUP
  {
    nsFileSystem::RegisterDataDirectoryFactory(nsDataDirectory::FileserveType::Factory, 100.0f);

    if (nsStartup::HasApplicationTag("tool") ||
        nsStartup::HasApplicationTag("testframework")) // the testframework configures a fileserve client itself
      return;

    nsFileserveClient* fs = nsFileserveClient::GetSingleton();

    if (fs == nullptr)
    {
      fs = NS_DEFAULT_NEW(nsFileserveClient);
      NS_IGNORE_UNUSED(fs);

      // on sandboxed platforms we must go through fileserve, so we enforce a fileserve connection
      // on unrestricted platforms, we use fileserve, if a connection can be established,
      // but if the connection times out, we fall back to regular file accesses
#if NS_DISABLED(NS_SUPPORTS_UNRESTRICTED_FILE_ACCESS)
      if (fs->SearchForServerAddress().Failed())
      {
        fs->WaitForServerInfo().IgnoreResult();
      }
#endif
    }
  }

  ON_CORESYSTEMS_SHUTDOWN
  {
    if (nsStartup::HasApplicationTag("tool") ||
        nsStartup::HasApplicationTag("testframework"))
      return;

    if (nsFileserveClient::GetSingleton() != nullptr)
    {
      nsFileserveClient* pSingleton = nsFileserveClient::GetSingleton();
      NS_DEFAULT_DELETE(pSingleton);
    }
  }

NS_END_SUBSYSTEM_DECLARATION;
// clang-format on


NS_STATICLINK_FILE(FileservePlugin, FileservePlugin_Main);
