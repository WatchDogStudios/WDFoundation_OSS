#pragma once

#include <EditorPluginFileserve/EditorPluginFileserveDLL.h>
#include <Foundation/Containers/Deque.h>
#include <Foundation/Containers/Map.h>
#include <QAbstractListModel>

class NS_EDITORPLUGINFILESERVE_DLL nsQtFileserveAllFilesModel : public QAbstractListModel
{
  Q_OBJECT

public:
  nsQtFileserveAllFilesModel(QWidget* pParent);

  virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  virtual QVariant data(const QModelIndex& index, int iRole = Qt::DisplayRole) const override;
  virtual QVariant headerData(int iSection, Qt::Orientation orientation, int iRole = Qt::DisplayRole) const override;

  void AddAccessedFile(const char* szFile);
  void UpdateView();

  void Clear();

private Q_SLOTS:
  void UpdateViewSlot();

private:
  bool m_bTimerRunning = false;
  nsUInt32 m_uiAddedItems = 0;
  nsMap<nsString, nsUInt32> m_AllFiles;
  nsDeque<nsMap<nsString, nsUInt32>::Iterator> m_IndexedFiles;
};
