#pragma once

#include <FileservePlugin/FileservePluginDLL.h>

#include <Foundation/Communication/RemoteInterface.h>
#include <Foundation/Configuration/Singleton.h>
#include <Foundation/Types/UniquePtr.h>
#include <Foundation/Types/Uuid.h>

namespace nsDataDirectory
{
  class FileserveType;
}

/// \brief Singleton that represents the client side part of a fileserve connection
///
/// Whether the fileserve plugin will be enabled is controled by nsFileserveClient::s_bEnableFileserve
/// By default this is on, but if switched off, the fileserve client functionality will be disabled.
/// nsFileserveClient will also switch its functionality off, if the command line argument "-fs_off" is specified.
/// If a program knows that it always wants to switch file serving off, it should either simply not load the plugin at all,
/// or it can inject that command line argument through nsCommandLineUtils. This should be done before application startup
/// and especially before any data directories get mounted.
///
/// The timeout for connecting to the server can be configured through the command line option "-fs_timeout seconds"
/// The server to connect to can be configured through command line option "-fs_server address".
/// The default address is "localhost:1042".
class NS_FILESERVEPLUGIN_DLL nsFileserveClient
{
  NS_DECLARE_SINGLETON(nsFileserveClient);

public:
  nsFileserveClient();
  ~nsFileserveClient();

  /// \brief Can be called at startup to go through multiple sources and search for a valid server address
  ///
  /// Ie. checks the command line, nsFileserve.txt in different directories, etc.
  /// For every potential IP it checks whether a fileserve connection could be established (e.g. tries to connect and
  /// checks whether the server answers). If a valid connection is found, the IP is stored internally and NS_SUCCESS is returned.
  /// Call GetServerConnectionAddress() to retrieve the address.
  ///
  /// \param timeout Specifies the timeout for checking whether a server can be reached.
  nsResult SearchForServerAddress(nsTime timeout = nsTime::MakeFromSeconds(5));

  /// \brief Waits for a Fileserver application to try to connect to this device and send its own information.
  ///
  /// This can be used when a device has no proper way to know the IP through which to connect to a Fileserver.
  /// Instead the device opens a server connection itself, and waits for the other side to try to connect to it.
  /// This typically means that a human has to manually input this device's IP on the host PC into the Fileserve application,
  /// thus enabling the exchange of connection information.
  /// Once this has happened, this function stores the valid server IP internally and returns with success.
  /// A subsequent call to EnsureConnected() should then succeed.
  nsResult WaitForServerInfo(nsTime timeout = nsTime::MakeFromSeconds(60.0 * 5));

  /// \brief Stores the current connection info to a text file in the user data folder.
  nsResult SaveCurrentConnectionInfoToDisk() const;

  /// \brief Allows to disable the file serving functionality. Should be called before mounting data directories.
  ///
  /// Also achieved through the command line argument "-fs_off"
  static void DisabledFileserveClient() { s_bEnableFileserve = false; }

  /// \brief Returns the address through which the Fileserve client tried to connect with the server last.
  const char* GetServerConnectionAddress() { return m_sServerConnectionAddress; }

  /// \brief Can be called to ensure a fileserve connection. Otherwise automatically called when a data directory is mounted.
  ///
  /// The timeout defines how long the code will wait for a connection.
  /// Positive numbers are a regular timeout.
  /// A zero timeout means the application will wait indefinitely.
  /// A negative number means to either wait that time, or whatever was specified through the command-line.
  /// The timeout can be specified with the command line switch "-fs_timeout X" (in seconds).
  nsResult EnsureConnected(nsTime timeout = nsTime::MakeFromSeconds(-5));

  /// \brief Needs to be called regularly to update the network. By default this is automatically called when the global event
  /// 'GameApp_UpdatePlugins' is fired, which is done by nsGameApplication.
  void UpdateClient();

  /// \brief Adds an address that should be tried for connecting with the server.
  void AddServerAddressToTry(nsStringView sAddress);

private:
  friend class nsDataDirectory::FileserveType;

  /// \brief True by default, can
  static bool s_bEnableFileserve;

  struct FileCacheStatus
  {
    nsInt64 m_TimeStamp = 0;
    nsUInt64 m_FileHash = 0;
    nsTime m_LastCheck;
  };

  struct DataDir
  {
    // nsString m_sRootName;
    // nsString m_sPathOnClient;
    nsString m_sMountPoint;
    bool m_bMounted = false;

    nsMap<nsString, FileCacheStatus> m_CacheStatus;
  };

  void DeleteFile(nsUInt16 uiDataDir, nsStringView sFile);
  nsUInt16 MountDataDirectory(nsStringView sDataDir, nsStringView sRootName);
  void UnmountDataDirectory(nsUInt16 uiDataDir);
  static void ComputeDataDirMountPoint(nsStringView sDataDir, nsStringBuilder& out_sMountPoint);
  void BuildPathInCache(const char* szFile, const char* szMountPoint, nsStringBuilder* out_pAbsPath, nsStringBuilder* out_pFullPathMeta) const;
  void GetFullDataDirCachePath(const char* szDataDir, nsStringBuilder& out_sFullPath, nsStringBuilder& out_sFullPathMeta) const;
  void NetworkMsgHandler(nsRemoteMessage& msg);
  void HandleFileTransferMsg(nsRemoteMessage& msg);
  void HandleFileTransferFinishedMsg(nsRemoteMessage& msg);
  static void WriteMetaFile(nsStringBuilder sCachedMetaFile, nsInt64 iFileTimeStamp, nsUInt64 uiFileHash);
  void WriteDownloadToDisk(nsStringBuilder sCachedFile);
  nsResult DownloadFile(nsUInt16 uiDataDirID, const char* szFile, bool bForceThisDataDir, nsStringBuilder* out_pFullPath);
  void DetermineCacheStatus(nsUInt16 uiDataDirID, const char* szFile, FileCacheStatus& out_Status) const;
  void UploadFile(nsUInt16 uiDataDirID, const char* szFile, const nsDynamicArray<nsUInt8>& fileContent);
  void InvalidateFileCache(nsUInt16 uiDataDirID, nsStringView sFile, nsUInt64 uiHash);
  static nsResult TryReadFileserveConfig(const char* szFile, nsStringBuilder& out_Result);
  nsResult TryConnectWithFileserver(const char* szAddress, nsTime timeout) const;
  void FillFileStatusCache(const char* szFile);
  void ShutdownConnection();
  void ClearState();

  mutable nsMutex m_Mutex;
  mutable nsString m_sServerConnectionAddress;
  nsString m_sFileserveCacheFolder;
  nsString m_sFileserveCacheMetaFolder;
  bool m_bDownloading = false;
  bool m_bFailedToConnect = false;
  bool m_bWaitingForUploadFinished = false;
  nsUuid m_CurFileRequestGuid;
  nsStringBuilder m_sCurFileRequest;
  nsUniquePtr<nsRemoteInterface> m_pNetwork;
  nsDynamicArray<nsUInt8> m_Download;
  nsTime m_CurrentTime;
  nsHybridArray<nsString, 4> m_TryServerAddresses;

  nsMap<nsString, nsUInt16> m_FileDataDir;
  nsHybridArray<DataDir, 8> m_MountedDataDirs;
};
