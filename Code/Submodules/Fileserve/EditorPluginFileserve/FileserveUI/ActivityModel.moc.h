#pragma once

#include <EditorPluginFileserve/EditorPluginFileserveDLL.h>
#include <Foundation/Containers/Deque.h>
#include <QAbstractListModel>

enum class nsFileserveActivityType
{
  StartServer,
  StopServer,
  ClientConnect,
  ClientReconnected,
  ClientDisconnect,
  Mount,
  MountFailed,
  Unmount,
  ReadFile,
  WriteFile,
  DeleteFile,
  Other
};

struct nsQtFileserveActivityItem
{
  QString m_Text;
  nsFileserveActivityType m_Type;
};

class NS_EDITORPLUGINFILESERVE_DLL nsQtFileserveActivityModel : public QAbstractListModel
{
  Q_OBJECT

public:
  nsQtFileserveActivityModel(QWidget* pParent);

  virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  virtual QVariant data(const QModelIndex& index, int iRole = Qt::DisplayRole) const override;
  virtual QVariant headerData(int iSection, Qt::Orientation orientation, int iRole = Qt::DisplayRole) const override;

  nsQtFileserveActivityItem& AppendItem();
  void UpdateView();

  void Clear();
private Q_SLOTS:
  void UpdateViewSlot();

private:
  bool m_bTimerRunning = false;
  nsUInt32 m_uiAddedItems = 0;
  nsDeque<nsQtFileserveActivityItem> m_Items;
};
