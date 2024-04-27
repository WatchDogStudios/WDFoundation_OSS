#include <EditorPluginFileserve/EditorPluginFileservePCH.h>

#include <EditorPluginFileserve/FileserveUI/ActivityModel.moc.h>

nsQtFileserveActivityModel::nsQtFileserveActivityModel(QWidget* pParent)
  : QAbstractListModel(pParent)
{
}

int nsQtFileserveActivityModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
  return m_Items.GetCount() - m_uiAddedItems;
}

int nsQtFileserveActivityModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const
{
  return 2;
}

QVariant nsQtFileserveActivityModel::data(const QModelIndex& index, int iRole /*= Qt::DisplayRole*/) const
{
  if (!index.isValid())
    return QVariant();

  const auto& item = m_Items[index.row()];

  if (iRole == Qt::ToolTipRole)
  {
    if (item.m_Type == nsFileserveActivityType::ReadFile)
    {
      return QString("[TIME] == File was not transferred because the timestamps match on server and client.\n"
                     "[HASH] == File was not transferred because the file hashes matched on server and client.\n"
                     "[N/A] == File does not exist on the server (in the requested data directory).");
    }
  }

  if (index.column() == 0)
  {
    if (iRole == Qt::DisplayRole)
    {
      switch (item.m_Type)
      {
        case nsFileserveActivityType::StartServer:
          return "Server Started";
        case nsFileserveActivityType::StopServer:
          return "Server Stopped";
        case nsFileserveActivityType::ClientConnect:
          return "Client Connected";
        case nsFileserveActivityType::ClientReconnected:
          return "Client Re-connected";
        case nsFileserveActivityType::ClientDisconnect:
          return "Client Disconnect";
        case nsFileserveActivityType::Mount:
          return "Mount";
        case nsFileserveActivityType::MountFailed:
          return "Failed Mount";
        case nsFileserveActivityType::Unmount:
          return "Unmount";
        case nsFileserveActivityType::ReadFile:
          return "Read";
        case nsFileserveActivityType::WriteFile:
          return "Write";
        case nsFileserveActivityType::DeleteFile:
          return "Delete";

        default:
          return QVariant();
      }
    }

    if (iRole == Qt::ForegroundRole)
    {
      switch (item.m_Type)
      {
        case nsFileserveActivityType::StartServer:
          return QColor::fromRgb(0, 200, 0);
        case nsFileserveActivityType::StopServer:
          return QColor::fromRgb(200, 200, 0);

        case nsFileserveActivityType::ClientConnect:
        case nsFileserveActivityType::ClientReconnected:
          return QColor::fromRgb(50, 200, 0);
        case nsFileserveActivityType::ClientDisconnect:
          return QColor::fromRgb(250, 100, 0);

        case nsFileserveActivityType::Mount:
          return QColor::fromRgb(0, 0, 200);
        case nsFileserveActivityType::MountFailed:
          return QColor::fromRgb(255, 0, 0);
        case nsFileserveActivityType::Unmount:
          return QColor::fromRgb(150, 0, 200);

        case nsFileserveActivityType::ReadFile:
          return QColor::fromRgb(100, 100, 100);
        case nsFileserveActivityType::WriteFile:
          return QColor::fromRgb(255, 150, 0);
        case nsFileserveActivityType::DeleteFile:
          return QColor::fromRgb(200, 50, 50);

        default:
          return QVariant();
      }
    }
  }

  if (index.column() == 1)
  {
    if (iRole == Qt::DisplayRole)
    {
      return item.m_Text;
    }
  }

  return QVariant();
}


QVariant nsQtFileserveActivityModel::headerData(int iSection, Qt::Orientation orientation, int iRole /*= Qt::DisplayRole*/) const
{
  if (iRole == Qt::DisplayRole)
  {
    if (iSection == 0)
    {
      return "Type";
    }

    if (iSection == 1)
    {
      return "Action";
    }
  }

  return QVariant();
}

nsQtFileserveActivityItem& nsQtFileserveActivityModel::AppendItem()
{
  if (!m_bTimerRunning)
  {
    m_bTimerRunning = true;

    QTimer::singleShot(250, this, &nsQtFileserveActivityModel::UpdateViewSlot);
  }

  m_uiAddedItems++;
  return m_Items.ExpandAndGetRef();
}

void nsQtFileserveActivityModel::UpdateView()
{
  if (m_uiAddedItems == 0)
    return;

  beginInsertRows(QModelIndex(), m_Items.GetCount() - m_uiAddedItems, m_Items.GetCount() - 1);
  insertRows(m_Items.GetCount(), m_uiAddedItems, QModelIndex());
  m_uiAddedItems = 0;
  endInsertRows();
}

void nsQtFileserveActivityModel::Clear()
{
  m_Items.Clear();
  m_uiAddedItems = 0;

  beginResetModel();
  endResetModel();
}

void nsQtFileserveActivityModel::UpdateViewSlot()
{
  m_bTimerRunning = false;

  UpdateView();
}
