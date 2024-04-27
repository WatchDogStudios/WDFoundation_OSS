#pragma once

#include <EditorPluginFileserve/EditorPluginFileserveDLL.h>
#include <EditorPluginFileserve/ui_FileserveWidget.h>
#include <Foundation/Containers/HashTable.h>
#include <Foundation/Time/Time.h>
#include <QWidget>

struct nsFileserverEvent;
class nsQtFileserveActivityModel;
class nsQtFileserveAllFilesModel;
enum class nsFileserveActivityType;

/// \brief A GUI for the nsFileServer
///
/// By default the file server does not run at startup. Using the command line option "-fs_start" the server is started immediately.
class NS_EDITORPLUGINFILESERVE_DLL nsQtFileserveWidget : public QWidget, public Ui_nsQtFileserveWidget
{
  Q_OBJECT

public:
  nsQtFileserveWidget(QWidget* pParent = nullptr);

  void FindOwnIP(nsStringBuilder& out_sDisplay, nsHybridArray<nsStringBuilder, 4>* out_pAllIPs = nullptr);

  ~nsQtFileserveWidget();

Q_SIGNALS:
  void ServerStarted(const QString& sIp, nsUInt16 uiPort);
  void ServerStopped();

public Q_SLOTS:
  void on_StartServerButton_clicked();
  void on_ClearActivityButton_clicked();
  void on_ClearAllFilesButton_clicked();
  void on_ReloadResourcesButton_clicked();
  void on_ConnectClient_clicked();

private:
  void FileserverEventHandler(const nsFileserverEvent& e);
  void LogActivity(const nsFormatString& text, nsFileserveActivityType type);
  void UpdateSpecialDirectoryUI();

  nsQtFileserveActivityModel* m_pActivityModel;
  nsQtFileserveAllFilesModel* m_pAllFilesModel;
  nsTime m_LastProgressUpdate;

  struct DataDirInfo
  {
    nsString m_sName;
    nsString m_sPath;
    nsString m_sRedirectedPath;
  };

  struct ClientData
  {
    bool m_bConnected = false;
    nsHybridArray<DataDirInfo, 8> m_DataDirs;
  };

  struct SpecialDir
  {
    nsString m_sName;
    nsString m_sPath;
  };

  nsHybridArray<SpecialDir, 4> m_SpecialDirectories;

  nsHashTable<nsUInt32, ClientData> m_Clients;
  void UpdateClientList();
  void ConfigureSpecialDirectories();
};
