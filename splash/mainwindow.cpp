#include "mainwindow.h"

#include "ui_mainwindow.h"
#include "ui_AnimationViewer.h"
#include "ImagePaletDialog/ImagePaletDialog.h"
#include "ResourceManager.h"
#include "AnimationViewer/AnimationViewer.h"
#include "QStandardItemModel.h"
#include "QTimelineWidget/QTimelinePanel.h"
#include <QDirModel>
#include <QFileDialog>
#include <QMessageBox>
#include "ResourceTree/ResourceTree.h"
#include "AnimationViewer/AnimationViewerPanel.h"

void MainWindow::setupConnections()
{
    connect(mpAnimationViewer->m_ui->showPaletteButton, SIGNAL(clicked()), this, SLOT(onPaletButtonClicked()));

    //Menu action
    connect(ui->actionSet_Animation_Directory, SIGNAL(triggered()), this, SLOT(setAnimationDirectory()));
    connect(ui->actionSet_Image_Directory, SIGNAL(triggered()), this, SLOT(setImageDirectory()));
    connect(ui->actionOpenFile, SIGNAL(triggered()), this, SLOT(selectProjectFile()));


    // connect animation list controls
    connect(ui->addAnimationButton, SIGNAL(clicked()), this, SLOT(onAddAnimationButtonClicked()));
    connect(ui->removeAnimationButton, SIGNAL(clicked()), this, SLOT(onRemoveAnimationButtonClicked()));

    connect(ui->addEventButton, SIGNAL(clicked()), this, SLOT(onAddEventButtonClicked()));
    connect(ui->removeEventButton, SIGNAL(clicked()), this, SLOT(onRemoveEventButtonClicked()));
    connect(ui->eventTableWidget, SIGNAL(cellChanged(int, int)), this, SLOT(onEventEdited(int, int)));

    // connect Animation Viewer
    connect(mpAnimationViewer, SIGNAL(playAnimation(bool)), this, SLOT(setEnabled(bool)));
    connect(ui->lineEditAnimationName, SIGNAL(textChanged(QString)), mpAnimationModel, SLOT(setAnimationName(QString)));
    connect(ui->lineEditAnimationID, SIGNAL(textChanged(QString)), mpAnimationModel, SLOT(setAnimationID(QString)));

    connect(mpAnimationModel, SIGNAL(animationNameChanged(QString)), ui->lineEditAnimationName, SLOT(setText(QString)));
    connect(mpAnimationModel, SIGNAL(animationTypeChanged(int)), ui->animationTypeComboBox, SLOT(setCurrentIndex(int)));
    connect(mpAnimationModel, SIGNAL(animationIDChanged(QString)), ui->lineEditAnimationID, SLOT(setText(QString)));
    connect(mpAnimationModel, SIGNAL(animationTypeChanged(int)), mpAnimationViewer, SLOT(onAnimationTypeChanged(int)));

    connect(ui->animationTreeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
        this, SLOT(onSelectionChanged(QItemSelection, QItemSelection)));
    connect(mpAnimationModel, SIGNAL(fileChanged()), this, SLOT(refreshTree()));

    // refresh button clicked
    connect(ui->treeRefreshButton, SIGNAL(clicked()), this, SLOT(refreshTree()));

    connect(mpAnimationModel, SIGNAL(selectedKeyFramePositionChanged(int, int)), this, SLOT(refreshEventList()));
    connect(mpAnimationModel, SIGNAL(refreshTimeLine()), this, SLOT(refreshEventList()));

    connect(ui->animationTypeComboBox, SIGNAL(currentIndexChanged(int)), mpAnimationModel, SLOT(setAnimationType(int)));
}

void MainWindow::setupModels()
{
    mpAnimationModel = new AnimationModel(this);
    mpAnimationViewer = new AnimationViewer(this, mpAnimationModel);
    mpAnimationListModel = new QStandardItemModel(0, 1, this);
}

void MainWindow::setupUIModels()
{
    mAnimationTreeViewModel.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    mAnimationTreeViewModel.setReadOnly(true);

    QString rootPath = ResourceManager::GetDirectoryPath(ResourceManager::FileType_Animation);

    ui->animationTreeView->setModel(&mAnimationTreeViewModel);
    ui->animationTreeView->setRootIndex(mAnimationTreeViewModel.index(rootPath) );
    ui->animationTreeView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
}
void MainWindow::setupUI()
{
    ui->setupUi(this);

    QStandardItemModel* animationTypeModel = new QStandardItemModel();
    // Set Tween types
    for (int i = 0; i < AnimationModel::AnimationType_COUNT; i++) {
      animationTypeModel->appendRow(new QStandardItem(AnimationModel::animationTypeSting[i]));
    }
    ui->animationTypeComboBox->setModel(animationTypeModel);

    mpAnimationViewer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->animationViewer->addWidget(mpAnimationViewer);

    mpQTimelinePanel = new QTimelinePanel(mpAnimationModel, this);
    mpQTimelinePanel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->timelineContainer->addWidget(mpQTimelinePanel);

    this->setFixedSize(1200, 800);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      mpDialog(NULL)
{
    ResourceManager::openSavedProject();
    setupModels();
    setupUI();

    setupUIModels();

    setupConnections();

    setEditEnabled(false);
}

MainWindow::~MainWindow()
{
    delete mpDialog;
    delete mpAnimationModel;
    delete mpAnimationListModel;
    delete ui;
}

void MainWindow::setEnabled(bool enabled)
{
    ui->eventsGroupdBox->setEnabled(enabled);
    ui->gridInfoBox->setEnabled(enabled);
    mpQTimelinePanel->setEnabled(enabled);

    mpAnimationViewer->m_ui->gridCelDataBox->setEnabled(false);
    ui->addAnimationButton->setEnabled(enabled);
    ui->removeAnimationButton->setEnabled(enabled);
    ui->animationTreeView->setEnabled(enabled);

}

void MainWindow::setEditEnabled(bool enabled)
{
    ui->eventsGroupdBox->setEnabled(enabled);
    ui->gridInfoBox->setEnabled(enabled);
    mpQTimelinePanel->setEnabled(enabled);
    mpAnimationViewer->setEnabled(enabled);
}

/* -------------------------------------------------------------------

 Animation data control

---------------------------------------------------------------------*/
void MainWindow::loadAnimationData(int index)
{

}

void MainWindow::onAnimationSelected(int index)
{
    loadAnimationData(index);
}

/* -------------------------------------------------------------------

 Animation treeview control

---------------------------------------------------------------------*/

static bool selectionCanceled = false;
void MainWindow::onSelectionChanged(const QItemSelection& item1, const QItemSelection& item2)
{
    if (selectionCanceled) {
        return;
    }

    QModelIndexList indexes = item1.indexes();
    QModelIndex index = indexes.takeFirst();

    QString path = mAnimationTreeViewModel.filePath(index);
    QFileInfo fileInfo = QFileInfo (path);
    if (fileInfo.isFile())
    {
        if (mpAnimationModel->isDataChanged()) {
            QMessageBox::StandardButton reply = QMessageBox::question(this, "Save", "The animation is not saved. \nDo you want to change anywys?",
                                          QMessageBox::Yes|QMessageBox::No);

            if (reply == QMessageBox::Yes) {
            }

            if (reply == QMessageBox::No) {
                selectionCanceled = true;
                if (item2.indexes().length() > 0) {
                    ui->animationTreeView->selectionModel()->clear();
                    ui->animationTreeView->selectionModel()->select(item2.indexes()[0], QItemSelectionModel::SelectionFlag::Select);
                    selectionCanceled = false;
                } else {
                    ui->animationTreeView->selectionModel()->clear();
                    selectionCanceled = false;
                }
                return;
            }
        }

        if (mpAnimationModel->loadData(path))
        {
            for(int i = ui->eventTableWidget->rowCount() - 1; i >= 0; i--)
            {
                ui->eventTableWidget->removeRow(i);
            }

            ResourceManager::clearAnimationCache();
            setEditEnabled(true);
        }
        else
        {
            setEditEnabled(false);
        }
    }
}

void MainWindow::onAddEventButtonClicked()
{
    KeyFrame::KeyFramePosition position = mpAnimationModel->getCurrentKeyFramePosition();
    mpAnimationModel->addEvent(position.mFrameNo);

    refreshEventList();
    mpAnimationModel->tellTimeLineToRefresh();
}

void MainWindow::onRemoveEventButtonClicked()
{
    KeyFrame::KeyFramePosition position = mpAnimationModel->getCurrentKeyFramePosition();
    QList<QTableWidgetSelectionRange> ranges = ui->eventTableWidget->selectedRanges();

    for(int i = 0; i < ranges.count(); i++)
    {
        QTableWidgetSelectionRange range = ranges[i];
        for(int j = range.bottomRow(); j >= range.topRow(); j--)
        {
            mpAnimationModel->removeEvent(position.mFrameNo, j);
        }
    }

    refreshEventList();
    mpAnimationModel->tellTimeLineToRefresh();
}

void MainWindow::onEventEdited(int row, int column)
{
    KeyFrame::KeyFramePosition position = mpAnimationModel->getCurrentKeyFramePosition();
    QString text = ui->eventTableWidget->item(row, column)->data(Qt::EditRole).toString();
    mpAnimationModel->setEventText(position.mFrameNo, row, text);
}

void MainWindow::refreshEventList()
{
    KeyFrame::KeyFramePosition position = mpAnimationModel->getCurrentKeyFramePosition();
    QList<QString> eventList = mpAnimationModel->getEventList(position.mFrameNo).mList;

    for(int i = ui->eventTableWidget->rowCount() - 1; i >= 0; i--)
    {
        ui->eventTableWidget->removeRow(i);
    }

    for(int i = eventList.count() - 1; i >= 0; i--)
    {
        ui->eventTableWidget->insertRow(0);
        ui->eventTableWidget->setItem(0, 0, new QTableWidgetItem(QString(eventList[i]), Qt::EditRole));
    }
}

void MainWindow::onAddAnimationButtonClicked()
{
    QModelIndexList indexes = ui->animationTreeView->selectionModel()->selectedIndexes();
    if (indexes.count() > 0)
    {
        QModelIndex index = indexes.takeFirst();

        QString path = mAnimationTreeViewModel.filePath(index);

        mpAnimationModel->createEmptyAnimation(path);
    }
}

void MainWindow::onRemoveAnimationButtonClicked()
{
    QModelIndexList indexes = ui->animationTreeView->selectionModel()->selectedIndexes();
    if (indexes.count() > 0)
    {

        QMessageBox::StandardButton reply = QMessageBox::question(this, "Delete", "Do you really want to remove this file? (Can't undo)",
                                      QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            QModelIndex index = indexes.takeFirst();

            QString path = mAnimationTreeViewModel.filePath(index);

            QFile::remove(path);
            refreshTree();
        }
    }
}

void MainWindow::onPaletButtonClicked()
{
    if (mpDialog != NULL) {
        delete mpDialog;
    }
    mpDialog = new ImagePaletDialog(this, mpAnimationModel);
    mpDialog->move(this->x(), this->y());
    mpDialog->setWindowFlag(Qt::WindowStaysOnTopHint);
    mpDialog->show();
}

void MainWindow::refreshTree()
{
    mAnimationTreeViewModel.refresh(QModelIndex());
}

void MainWindow::setAnimationDirectory()
{
    QString newDirectory = QFileDialog::getExistingDirectory
                    (
                        this,
                        tr("Open"),
                        ""
                    );

    ResourceManager::setWorkingDirectory(newDirectory, ResourceManager::FileType_Animation);

    QString rootPath = ResourceManager::GetDirectoryPath(ResourceManager::FileType_Animation);
    printf("ROOT PAH %s", rootPath.toStdString().c_str());
    ui->animationTreeView->setRootIndex(mAnimationTreeViewModel.index(rootPath) );
}

void MainWindow::setImageDirectory()
{
    QString newDirectory = QFileDialog::getExistingDirectory
                    (
                        this,
                        tr("Open"),
                        ""
                    );

    ResourceManager::setWorkingDirectory(newDirectory, ResourceManager::FileType_Image);
}


void MainWindow::selectProjectFile()
{
    QString filename = QFileDialog::getOpenFileName(
                        this,
                        tr("Open"),
                        "~/",
                        tr("XML files (*.spl)")
                    );

    if (filename == NULL) {
        return;
    }

    ResourceManager::setProjectPath(filename);

    QString rootPath = ResourceManager::GetDirectoryPath(ResourceManager::FileType_Animation);
    ui->animationTreeView->setRootIndex(mAnimationTreeViewModel.index(rootPath) );
}


