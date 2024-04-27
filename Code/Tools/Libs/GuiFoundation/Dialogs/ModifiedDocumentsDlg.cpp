#include <GuiFoundation/GuiFoundationPCH.h>

#include <Foundation/Strings/TranslationLookup.h>
#include <GuiFoundation/Dialogs/ModifiedDocumentsDlg.moc.h>
#include <GuiFoundation/UIServices/UIServices.moc.h>
#include <ToolsFoundation/Project/ToolsProject.h>

nsQtModifiedDocumentsDlg::nsQtModifiedDocumentsDlg(QWidget* pParent, const nsHybridArray<nsDocument*, 32>& modifiedDocs)
  : QDialog(pParent)
{
  m_ModifiedDocs = modifiedDocs;

  setupUi(this);

  TableDocuments->blockSignals(true);

  TableDocuments->setRowCount(m_ModifiedDocs.GetCount());

  QStringList Headers;
  Headers.append(" Type ");
  Headers.append(" Document ");
  Headers.append("");

  TableDocuments->setColumnCount(static_cast<int>(Headers.size()));

  TableDocuments->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
  TableDocuments->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
  TableDocuments->setHorizontalHeaderLabels(Headers);
  TableDocuments->horizontalHeader()->show();
  TableDocuments->setSortingEnabled(true);
  TableDocuments->horizontalHeader()->setStretchLastSection(false);
  TableDocuments->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::ResizeToContents);
  TableDocuments->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Stretch);
  TableDocuments->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeMode::Fixed);

  NS_VERIFY(connect(TableDocuments, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(SlotSelectionChanged(int, int, int, int))) != nullptr, "signal/slot connection failed");

  nsInt32 iRow = 0;
  for (nsDocument* pDoc : m_ModifiedDocs)
  {
    nsString sText = pDoc->GetDocumentPath();

    if (!nsToolsProject::GetSingleton()->IsDocumentInAllowedRoot(pDoc->GetDocumentPath(), &sText))
      sText = pDoc->GetDocumentPath();

    QPushButton* pButtonSave = new QPushButton(QLatin1String("Save"));
    NS_VERIFY(connect(pButtonSave, SIGNAL(clicked()), this, SLOT(SlotSaveDocument())) != nullptr, "signal/slot connection failed");

    pButtonSave->setIcon(QIcon(":/GuiFoundation/Icons/Save.svg"));
    pButtonSave->setProperty("document", QVariant::fromValue((void*)pDoc));

    pButtonSave->setMinimumWidth(100);
    pButtonSave->setMaximumWidth(100);

    TableDocuments->setCellWidget(iRow, 2, pButtonSave);

    QTableWidgetItem* pItem0 = new QTableWidgetItem();
    pItem0->setData(Qt::DisplayRole, nsMakeQString(nsTranslate(pDoc->GetDocumentTypeDescriptor()->m_sDocumentTypeName)));
    pItem0->setIcon(nsQtUiServices::GetCachedIconResource(pDoc->GetDocumentTypeDescriptor()->m_sIcon));
    TableDocuments->setItem(iRow, 0, pItem0);

    QTableWidgetItem* pItem1 = new QTableWidgetItem();
    pItem1->setData(Qt::DisplayRole, QString::fromUtf8(sText.GetData()));
    TableDocuments->setItem(iRow, 1, pItem1);

    ++iRow;
  }

  TableDocuments->resizeColumnsToContents();
  TableDocuments->blockSignals(false);
}

nsResult nsQtModifiedDocumentsDlg::SaveDocument(nsDocument* pDoc)
{
  if (!pDoc->IsModified())
    return NS_SUCCESS;

  {
    if (pDoc->GetUnknownObjectTypeInstances() > 0)
    {
      if (nsQtUiServices::MessageBoxQuestion("Warning! This document contained unknown object types that could not be loaded. Saving the "
                                             "document means those objects will get lost permanently.\n\nDo you really want to save this "
                                             "document?",
            QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No, QMessageBox::StandardButton::No) != QMessageBox::StandardButton::Yes)
        return NS_SUCCESS; // failed successfully
    }
  }

  auto res = pDoc->SaveDocument();

  if (res.m_Result.Failed())
  {
    nsStringBuilder s, s2;
    s.SetFormat("Failed to save document:\n'{0}'", pDoc->GetDocumentPath());
    s2.SetFormat("Successfully saved document:\n'{0}'", pDoc->GetDocumentPath());

    nsQtUiServices::MessageBoxStatus(res, s, s2);

    return NS_FAILURE;
  }

  return NS_SUCCESS;
}

void nsQtModifiedDocumentsDlg::SlotSaveDocument()
{
  QPushButton* pButtonSave = qobject_cast<QPushButton*>(sender());

  if (!pButtonSave)
    return;

  nsDocument* pDoc = (nsDocument*)pButtonSave->property("document").value<void*>();

  SaveDocument(pDoc).IgnoreResult();

  pButtonSave->setEnabled(pDoc->IsModified());

  // Check if now all documents are saved and close the dialog if so
  bool anyDocumentModified = false;
  for (nsDocument* pDoc2 : m_ModifiedDocs)
  {
    if (pDoc2->IsModified())
    {
      anyDocumentModified = true;
      break;
    }
  }

  if (!anyDocumentModified)
  {
    accept();
  }
}

void nsQtModifiedDocumentsDlg::SlotSelectionChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
  QPushButton* pButtonSave = qobject_cast<QPushButton*>(TableDocuments->cellWidget(currentRow, 2));

  if (!pButtonSave)
    return;

  nsDocument* pDoc = (nsDocument*)pButtonSave->property("document").value<void*>();

  pDoc->EnsureVisible();
}

void nsQtModifiedDocumentsDlg::on_ButtonSaveSelected_clicked()
{
  for (nsDocument* pDoc : m_ModifiedDocs)
  {
    if (SaveDocument(pDoc).Failed())
      return;
  }

  accept();
}

void nsQtModifiedDocumentsDlg::on_ButtonDontSave_clicked()
{
  accept();
}
