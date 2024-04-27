#include <EditorPluginFileserve/EditorPluginFileservePCH.h>

#include <EditorPluginFileserve/FileserveUI/ActivityModel.moc.h>
#include <EditorPluginFileserve/FileserveUI/AllFilesModel.moc.h>
#include <EditorPluginFileserve/FileserveUI/FileserveWidget.moc.h>
#include <Foundation/Utilities/CommandLineUtils.h>

nsQtFileserveWidget::nsQtFileserveWidget(QWidget* pParent /*= nullptr*/)
{
  setupUi(this);
  Progress->reset();
  m_pActivityModel = new nsQtFileserveActivityModel(this);
  m_pAllFilesModel = new nsQtFileserveAllFilesModel(this);

  ActivityList->setModel(m_pActivityModel);
  AllFilesList->setModel(m_pAllFilesModel);

  ActivityList->horizontalHeader()->setVisible(true);
  AllFilesList->horizontalHeader()->setVisible(true);

  {
    ClientsList->setColumnCount(3);

    QStringList header;
    header.append("");
    header.append("");
    header.append("");
    ClientsList->setHeaderLabels(header);
    ClientsList->setHeaderHidden(false);
  }

  {
    QHeaderView* verticalHeader = ActivityList->verticalHeader();
    verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader->setDefaultSectionSize(24);
  }

  {
    QHeaderView* verticalHeader = AllFilesList->verticalHeader();
    verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader->setDefaultSectionSize(24);
  }

  connect(m_pActivityModel, SIGNAL(rowsInserted(QModelIndex, int, int)), ActivityList, SLOT(scrollToBottom()));

  if (nsFileserver::GetSingleton())
  {
    nsFileserver::GetSingleton()->m_Events.AddEventHandler(nsMakeDelegate(&nsQtFileserveWidget::FileserverEventHandler, this));
    const nsUInt16 uiPort = nsFileserver::GetSingleton()->GetPort();

    PortLineEdit->setText(QString::number(uiPort));
  }
  else
  {
    setEnabled(false);
  }

  {
    nsStringBuilder sDisplayString;
    FindOwnIP(sDisplayString);

    IpLabel->setText(sDisplayString.GetData());
  }

  ReloadResourcesButton->setEnabled(false);
  SpecialDirAddButton->setVisible(false);
  SpecialDirBrowseButton->setVisible(false);
  SpecialDirRemoveButton->setVisible(false);

  SpecialDirList->setToolTip("Special directories allow to redirect mount requests from the client to a specific folder on the server.\n\n"
                             "Some special directories are built in (e.g. 'sdk', 'user' and 'appdir') but you can add custom ones, if your app needs one.\n"
                             "To add special directories, run Fileserve with the command line argument '-specialdirs' followed by the name and the path to a "
                             "directory.\n\n"
                             "For instance:\n"
                             "-specialdirs project \"C:\\path\\to\\project\" secondDir \"d:\\another\\path\"");

  ConfigureSpecialDirectories();

  UpdateSpecialDirectoryUI();

  if (nsCommandLineUtils::GetGlobalInstance()->GetBoolOption("-fs_start"))
  {
    QTimer::singleShot(100, this, &nsQtFileserveWidget::on_StartServerButton_clicked);
  }
}

void nsQtFileserveWidget::FindOwnIP(nsStringBuilder& out_sDisplay, nsHybridArray<nsStringBuilder, 4>* out_pAllIPs)
{
  nsStringBuilder hardwarename;
  out_sDisplay.Clear();

  for (const QNetworkInterface& neti : QNetworkInterface::allInterfaces())
  {
    hardwarename = neti.humanReadableName().toUtf8().data();

    if (!neti.isValid())
      continue;
    if (neti.flags().testFlag(QNetworkInterface::IsLoopBack))
      continue;

    if (!neti.flags().testFlag(QNetworkInterface::IsUp))
      continue;
    if (!neti.flags().testFlag(QNetworkInterface::IsRunning))
      continue;
    if (!neti.flags().testFlag(QNetworkInterface::CanBroadcast))
      continue;

    for (const QNetworkAddressEntry& entry : neti.addressEntries())
    {
      if (entry.ip().protocol() != QAbstractSocket::IPv4Protocol)
        continue;
      if (entry.ip().isLoopback())
        continue;
      if (entry.ip().isMulticast())
        continue;
      if (entry.ip().isNull())
        continue;

      // if we DO find multiple adapters, display them all
      if (!out_sDisplay.IsEmpty())
        out_sDisplay.Append("\n");

      out_sDisplay.AppendFormat("Adapter: '{0}' = {1}", hardwarename, entry.ip().toString().toUtf8().data());

      if (out_pAllIPs != nullptr)
      {
        out_pAllIPs->PushBack(entry.ip().toString().toUtf8().data());
      }
    }
  }
}

nsQtFileserveWidget::~nsQtFileserveWidget()
{
  if (nsFileserver::GetSingleton())
  {
    nsFileserver::GetSingleton()->m_Events.AddEventHandler(nsMakeDelegate(&nsQtFileserveWidget::FileserverEventHandler, this));
  }
}

void nsQtFileserveWidget::on_StartServerButton_clicked()
{
  if (nsFileserver::GetSingleton())
  {
    if (nsFileserver::GetSingleton()->IsServerRunning())
    {
      if (QMessageBox::question(this, "Stop Server?", "Stop Server?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No)
        return;

      nsFileserver::GetSingleton()->StopServer();
    }
    else
    {
      QString sPort = PortLineEdit->text();
      bool bOk = false;
      nsUInt32 uiPort = sPort.toUInt(&bOk);

      if (bOk && uiPort <= 0xFFFF)
      {
        nsFileserver::GetSingleton()->SetPort((nsUInt16)uiPort);
        nsFileserver::GetSingleton()->StartServer();
      }
      else
        QMessageBox::information(this, "Invalid Port", "The port must be a number between 0 and 65535", QMessageBox::Ok, QMessageBox::Ok);
    }
  }
}

void nsQtFileserveWidget::on_ClearActivityButton_clicked()
{
  m_pActivityModel->Clear();
}

void nsQtFileserveWidget::on_ClearAllFilesButton_clicked()
{
  m_pAllFilesModel->Clear();
}


void nsQtFileserveWidget::on_ReloadResourcesButton_clicked()
{
  if (nsFileserver::GetSingleton())
  {
    nsFileserver::GetSingleton()->BroadcastReloadResourcesCommand();
  }
}

void nsQtFileserveWidget::on_ConnectClient_clicked()
{
  QString sIP;

  {
    QSettings Settings;
    Settings.beginGroup(QLatin1String("Fileserve"));
    sIP = Settings.value("ConnectClientIP", "").toString();
    Settings.endGroup();
  }

  bool ok = false;
  sIP = QInputDialog::getText(this, "Connect to Device", "Device IP:", QLineEdit::Normal, sIP, &ok);
  if (!ok)
    return;

  {
    QSettings Settings;
    Settings.beginGroup(QLatin1String("Fileserve"));
    Settings.setValue("ConnectClientIP", sIP);
    Settings.endGroup();
  }

  nsStringBuilder sDisplayString;
  nsHybridArray<nsStringBuilder, 4> AllIPs;
  FindOwnIP(sDisplayString, &AllIPs);

  if (nsFileserver::SendConnectionInfo(sIP.toUtf8().data(), PortLineEdit->text().toInt(), AllIPs).Succeeded())
  {
    LogActivity(nsFmt("Successfully sent server info to client at '{0}'", sIP.toUtf8().data()), nsFileserveActivityType::Other);
  }
  else
  {
    LogActivity(nsFmt("Failed to connect with client at '{0}'", sIP.toUtf8().data()), nsFileserveActivityType::Other);
  }
}

void nsQtFileserveWidget::FileserverEventHandler(const nsFileserverEvent& e)
{
  switch (e.m_Type)
  {
    case nsFileserverEvent::Type::None:
      NS_ASSERT_DEV(false, "None event should never be fired");
      break;

    case nsFileserverEvent::Type::ServerStarted:
    {
      LogActivity("", nsFileserveActivityType::StartServer);
      PortLineEdit->setEnabled(false);
      ReloadResourcesButton->setEnabled(true);
      StartServerButton->setText("Stop Server");

      nsStringBuilder sDisplayString;
      FindOwnIP(sDisplayString);

      Q_EMIT ServerStarted(sDisplayString.GetData(), nsFileserver::GetSingleton()->GetPort());
    }
    break;

    case nsFileserverEvent::Type::ServerStopped:
    {
      LogActivity("", nsFileserveActivityType::StopServer);
      PortLineEdit->setEnabled(true);
      ReloadResourcesButton->setEnabled(false);
      StartServerButton->setText("Start Server");

      Q_EMIT ServerStopped();
    }
    break;

    case nsFileserverEvent::Type::ClientConnected:
    {
      LogActivity("", nsFileserveActivityType::ClientConnect);
      m_Clients[e.m_uiClientID].m_bConnected = true;

      UpdateClientList();
    }
    break;

    case nsFileserverEvent::Type::ClientReconnected:
    {
      LogActivity("", nsFileserveActivityType::ClientReconnected);
      m_Clients[e.m_uiClientID].m_bConnected = true;

      UpdateClientList();
    }
    break;

    case nsFileserverEvent::Type::ClientDisconnected:
    {
      LogActivity("", nsFileserveActivityType::ClientDisconnect);
      m_Clients[e.m_uiClientID].m_bConnected = false;

      UpdateClientList();
    }
    break;

    case nsFileserverEvent::Type::MountDataDir:
    {
      LogActivity(e.m_szPath, nsFileserveActivityType::Mount);

      DataDirInfo& dd = m_Clients[e.m_uiClientID].m_DataDirs.ExpandAndGetRef();
      dd.m_sName = e.m_szName;
      dd.m_sPath = e.m_szPath;
      dd.m_sRedirectedPath = e.m_szRedirectedPath;

      UpdateClientList();
    }
    break;

    case nsFileserverEvent::Type::MountDataDirFailed:
    {
      LogActivity(e.m_szPath, nsFileserveActivityType::MountFailed);

      DataDirInfo& dd = m_Clients[e.m_uiClientID].m_DataDirs.ExpandAndGetRef();
      dd.m_sName = e.m_szName;
      dd.m_sPath = e.m_szPath;
      dd.m_sRedirectedPath = "Failed to mount this directory. Special directory name unknown.";

      UpdateClientList();
    }
    break;

    case nsFileserverEvent::Type::UnmountDataDir:
    {
      LogActivity(e.m_szName, nsFileserveActivityType::Unmount);

      auto& dds = m_Clients[e.m_uiClientID].m_DataDirs;
      for (nsUInt32 i = 0; i < dds.GetCount(); ++i)
      {
        if (dds[i].m_sName == e.m_szName)
        {
          dds.RemoveAtAndCopy(i);
          break;
        }
      }

      UpdateClientList();
    }
    break;

    case nsFileserverEvent::Type::FileDownloadRequest:
    {
      m_pAllFilesModel->AddAccessedFile(e.m_szPath);
      TransferLabel->setText(QString("Downloading: %1").arg(e.m_szPath));
      m_LastProgressUpdate = nsTime::Now();

      if (e.m_FileState == nsFileserveFileState::NonExistant)
        LogActivity(nsFmt("[N/A] {0}", e.m_szPath), nsFileserveActivityType::ReadFile);

      if (e.m_FileState == nsFileserveFileState::SameHash)
        LogActivity(nsFmt("[HASH] {0}", e.m_szPath), nsFileserveActivityType::ReadFile);

      if (e.m_FileState == nsFileserveFileState::SameTimestamp)
        LogActivity(nsFmt("[TIME] {0}", e.m_szPath), nsFileserveActivityType::ReadFile);

      if (e.m_FileState == nsFileserveFileState::NonExistantEither)
        LogActivity(nsFmt("[N/A] {0}", e.m_szPath), nsFileserveActivityType::ReadFile);

      if (e.m_FileState == nsFileserveFileState::Different)
        LogActivity(nsFmt("({1} KB) {0}", e.m_szPath, nsArgF(e.m_uiSizeTotal / 1024.0f, 1)), nsFileserveActivityType::ReadFile);
    }
    break;

    case nsFileserverEvent::Type::FileDownloading:
    {
      if (nsTime::Now() - m_LastProgressUpdate > nsTime::MakeFromMilliseconds(100))
      {
        m_LastProgressUpdate = nsTime::Now();
        Progress->setValue((int)(100.0 * e.m_uiSentTotal / e.m_uiSizeTotal));
      }
    }
    break;

    case nsFileserverEvent::Type::FileDownloadFinished:
    {
      TransferLabel->setText(QString());
      Progress->reset();
    }
    break;

    case nsFileserverEvent::Type::FileDeleteRequest:
      LogActivity(e.m_szPath, nsFileserveActivityType::DeleteFile);
      break;

    case nsFileserverEvent::Type::FileUploadRequest:
    {
      LogActivity(nsFmt("({1} KB) {0}", e.m_szPath, nsArgF(e.m_uiSizeTotal / 1024.0f, 1)), nsFileserveActivityType::WriteFile);
      TransferLabel->setText(QString("Uploading: %1").arg(e.m_szPath));
      m_LastProgressUpdate = nsTime::Now();
    }
    break;

    case nsFileserverEvent::Type::FileUploading:
    {
      if (nsTime::Now() - m_LastProgressUpdate > nsTime::MakeFromMilliseconds(100))
      {
        m_LastProgressUpdate = nsTime::Now();
        Progress->setValue((int)(100.0 * e.m_uiSentTotal / e.m_uiSizeTotal));
      }
    }
    break;

    case nsFileserverEvent::Type::FileUploadFinished:
    {
      TransferLabel->setText(QString());
      Progress->reset();
    }
    break;

    case nsFileserverEvent::Type::AreYouThereRequest:
    {
      LogActivity("Client searching for Server", nsFileserveActivityType::Other);
    }
    break;
  }
}

void nsQtFileserveWidget::LogActivity(const nsFormatString& text, nsFileserveActivityType type)
{
  auto& item = m_pActivityModel->AppendItem();

  nsStringBuilder tmp;
  item.m_Text = text.GetTextCStr(tmp);
  item.m_Type = type;
}

void nsQtFileserveWidget::UpdateSpecialDirectoryUI()
{
  QTableWidget* pTable = SpecialDirList;
  nsQtScopedBlockSignals bs(SpecialDirList);

  QStringList header;
  header.append("Special Directory");
  header.append("Path");

  SpecialDirList->clear();
  pTable->setColumnCount(2);
  pTable->setRowCount(m_SpecialDirectories.GetCount() + 3);
  pTable->horizontalHeader()->setStretchLastSection(true);
  pTable->verticalHeader()->setDefaultSectionSize(24);
  pTable->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
  pTable->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
  pTable->verticalHeader()->setHidden(true);
  pTable->setHorizontalHeaderLabels(header);

  nsStringBuilder sResolved;
  QTableWidgetItem* pItem;

  nsUInt32 row = 0;

  for (nsUInt32 i = 0; i < m_SpecialDirectories.GetCount(); ++i, ++row)
  {
    pItem = new QTableWidgetItem();
    pItem->setText(m_SpecialDirectories[i].m_sName.GetData());
    pItem->setFlags(Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable);
    pTable->setItem(row, 0, pItem);

    pItem = new QTableWidgetItem();
    pItem->setText(m_SpecialDirectories[i].m_sPath.GetData());
    pItem->setFlags(Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable);
    pTable->setItem(row, 1, pItem);
  }

  {
    nsFileSystem::ResolveSpecialDirectory(">sdk", sResolved).IgnoreResult();

    pItem = new QTableWidgetItem();
    pItem->setText("sdk");
    pItem->setFlags(Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable);
    pTable->setItem(row, 0, pItem);

    pItem = new QTableWidgetItem();
    pItem->setText(sResolved.GetData());
    pItem->setFlags(Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable);
    pTable->setItem(row, 1, pItem);

    ++row;
  }

  {
    nsFileSystem::ResolveSpecialDirectory(">user", sResolved).IgnoreResult();

    pItem = new QTableWidgetItem();
    pItem->setText("user");
    pItem->setFlags(Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable);
    pTable->setItem(row, 0, pItem);

    pItem = new QTableWidgetItem();
    pItem->setText(sResolved.GetData());
    pItem->setFlags(Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable);
    pTable->setItem(row, 1, pItem);

    ++row;
  }

  {
    nsFileSystem::ResolveSpecialDirectory(">appdir", sResolved).IgnoreResult();

    pItem = new QTableWidgetItem();
    pItem->setText("appdir");
    pItem->setFlags(Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable);
    pTable->setItem(row, 0, pItem);

    pItem = new QTableWidgetItem();
    pItem->setText(sResolved.GetData());
    pItem->setFlags(Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable);
    pTable->setItem(row, 1, pItem);

    ++row;
  }
}

void nsQtFileserveWidget::UpdateClientList()
{
  nsQtScopedBlockSignals bs(ClientsList);

  ClientsList->clear();

  nsStringBuilder sName;

  for (auto it = m_Clients.GetIterator(); it.IsValid(); ++it)
  {
    QTreeWidgetItem* pClient = new QTreeWidgetItem();

    sName.SetFormat("Client: {0}", it.Key());
    pClient->setText(0, sName.GetData());
    pClient->setText(1, it.Value().m_bConnected ? "connected" : "disconnected");

    ClientsList->addTopLevelItem(pClient);

    for (const DataDirInfo& dd : it.Value().m_DataDirs)
    {
      QTreeWidgetItem* pDir = new QTreeWidgetItem(pClient);

      sName = dd.m_sName.GetData();
      if (!sName.IsEmpty())
        sName.Prepend(":");

      pDir->setText(0, sName.GetData());
      pDir->setText(1, dd.m_sPath.GetData());
      pDir->setText(2, dd.m_sRedirectedPath.GetData());

      if (dd.m_sRedirectedPath.StartsWith("Failed"))
        pDir->setForeground(2, QColor::fromRgb(255, 0, 0));
    }

    pClient->setExpanded(it.Value().m_bConnected);
  }

  ClientsList->resizeColumnToContents(2);
  ClientsList->resizeColumnToContents(1);
  ClientsList->resizeColumnToContents(0);
}

void nsQtFileserveWidget::ConfigureSpecialDirectories()
{
  const auto pCmd = nsCommandLineUtils::GetGlobalInstance();
  const nsUInt32 uiArgs = pCmd->GetStringOptionArguments("-specialdirs");

  nsStringBuilder sDir, sPath;

  for (nsUInt32 i = 0; i < uiArgs; i += 2)
  {
    sDir = pCmd->GetStringOption("-specialdirs", i, "");
    sPath = pCmd->GetStringOption("-specialdirs", i + 1, "");

    if (sDir.IsEmpty() || sPath.IsEmpty())
      continue;

    nsFileSystem::SetSpecialDirectory(sDir, sPath);
    sPath.MakeCleanPath();

    auto& sd = m_SpecialDirectories.ExpandAndGetRef();
    sd.m_sName = sDir;
    sd.m_sPath = sPath;
  }
}
