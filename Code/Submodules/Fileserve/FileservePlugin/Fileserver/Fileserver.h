#pragma once

#include <FileservePlugin/Fileserver/ClientContext.h>
#include <Foundation/Communication/RemoteInterface.h>
#include <Foundation/Configuration/Singleton.h>
#include <Foundation/Containers/HashTable.h>
#include <Foundation/Types/UniquePtr.h>
#include <Foundation/Types/Uuid.h>

class nsRemoteMessage;

struct nsFileserverEvent
{
  enum class Type
  {
    None,
    ServerStarted,
    ServerStopped,
    ClientConnected,
    ClientReconnected, // connected again after a disconnect
    ClientDisconnected,
    MountDataDir,
    MountDataDirFailed,
    UnmountDataDir,
    FileDownloadRequest,
    FileDownloading,
    FileDownloadFinished,
    FileDeleteRequest,
    FileUploadRequest,
    FileUploading,
    FileUploadFinished,
    AreYouThereRequest,
  };

  Type m_Type = Type::None;
  nsUInt32 m_uiClientID = 0;
  const char* m_szName = nullptr;
  const char* m_szPath = nullptr;
  const char* m_szRedirectedPath = nullptr;
  nsUInt32 m_uiSizeTotal = 0;
  nsUInt32 m_uiSentTotal = 0;
  nsFileserveFileState m_FileState = nsFileserveFileState::None;
};

/// \brief A file server allows to serve files from a host PC to another process that is potentially on another device.
///
/// This is mostly useful for mobile devices, that do not have access to the data on the development machine.
/// Typically every change to a file would require packaging the app and deploying it to the device again.
/// Fileserve allows to only deploy a very lean application and instead get all asset data directly from a host PC.
/// This also allows to modify data on the PC and reload the data in the running application without delay.
///
/// A single file server can serve multiple clients. However, to mount "special directories" (see nsFileSystem) the server
/// needs to know what local path to map them to (it uses the configuration on nsFileSystem).
/// That means it cannot serve two clients that require different settings for the same special directory.
///
/// The port on which the server connects to clients can be configured through the command line option "-fs_port X"
class NS_FILESERVEPLUGIN_DLL nsFileserver
{
  NS_DECLARE_SINGLETON(nsFileserver);

public:
  nsFileserver();

  /// \brief Starts listening for client connections. Uses the configured port.
  void StartServer();

  /// \brief Disconnects all clients.
  void StopServer();

  /// \brief Has to be executed regularly to serve clients and keep the connection alive.
  bool UpdateServer();

  /// \brief Whether the server was started.
  bool IsServerRunning() const;

  /// \brief Overrides the current port setting. May only be called when the server is currently not running.
  void SetPort(nsUInt16 uiPort);

  /// \brief Returns the currently set port. If the command line option "-fs_port X" was used, this will return that value, otherwise the default is
  /// 1042.
  nsUInt16 GetPort() const { return m_uiPort; }

  /// \brief The server broadcasts events about its activity
  nsEvent<const nsFileserverEvent&> m_Events;

  /// \brief Broadcasts to all clients that they should reload their resources
  void BroadcastReloadResourcesCommand();

  static nsResult SendConnectionInfo(
    const char* szClientAddress, nsUInt16 uiMyPort, const nsArrayPtr<nsStringBuilder>& myIPs, nsTime timeout = nsTime::MakeFromSeconds(10));

private:
  void NetworkEventHandler(const nsRemoteEvent& e);
  nsFileserveClientContext& DetermineClient(nsRemoteMessage& msg);
  void NetworkMsgHandler(nsRemoteMessage& msg);
  void HandleMountRequest(nsFileserveClientContext& client, nsRemoteMessage& msg);
  void HandleUnmountRequest(nsFileserveClientContext& client, nsRemoteMessage& msg);
  void HandleFileRequest(nsFileserveClientContext& client, nsRemoteMessage& msg);
  void HandleDeleteFileRequest(nsFileserveClientContext& client, nsRemoteMessage& msg);
  void HandleUploadFileHeader(nsFileserveClientContext& client, nsRemoteMessage& msg);
  void HandleUploadFileTransfer(nsFileserveClientContext& client, nsRemoteMessage& msg);
  void HandleUploadFileFinished(nsFileserveClientContext& client, nsRemoteMessage& msg);

  nsHashTable<nsUInt32, nsFileserveClientContext> m_Clients;
  nsUniquePtr<nsRemoteInterface> m_pNetwork;
  nsDynamicArray<nsUInt8> m_SendToClient;   // ie. 'downloads' from server to client
  nsDynamicArray<nsUInt8> m_SentFromClient; // ie. 'uploads' from client to server
  nsStringBuilder m_sCurFileUpload;
  nsUuid m_FileUploadGuid;
  nsUInt32 m_uiFileUploadSize;
  nsUInt16 m_uiPort = 1042;
};
