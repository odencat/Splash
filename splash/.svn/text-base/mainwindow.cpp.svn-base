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
#include "ResourceTree/ResourceTree.h"
#include "AnimationViewer/AnimationViewerPanel.h"

void MainWindow::setupConnections()
{
    connect(mpAnimationViewer->m_ui->showPaletteButton, SIGNAL(clicked()), this, SLOT(onPaletButtonClicked()));

    //Menu action
    connect(ui->actionSet_Working_Directory, SIGNAL(triggered()), this, SLOT(setWorkingDirectory()));

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
    connect(mpAnimationModel, SIGNAL(animationIDChanged(QString)), ui->lineEditAnimationID, SLOT(setText(QString)));

    connect(ui->animationTreeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
        this, SLOT(onSelectionChanged(QItemSelection, QItemSelection)));
    connect(mpAnimationModel, SIGNAL(fileChanged()), this, SLOT(refreshTree()));

    // refresh button clicked
    connect(ui->treeRefreshButton, SIGNAL(clicked()), this, SLOT(refreshTree()));

    connect(mpAnimationModel, SIGNAL(selectedKeyFramePositionChanged(int, int)), this, SLOT(refreshEventList()));
    connect(mpAnimationModel, SIGNAL(refreshTimeLine()), this, SLOT(refreshEventList()));
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

    QString rootPath = ResourceManager::getResourcePath(ANIMATION_DIR.path());

    ui->animationTreeView->setAutoScroll(true);
    ui->animationTreeView->setModel(&mAnimationTreeViewModel);
    ui->animationTreeView->setRootIndex(mAnimationTreeViewModel.index(rootPath) );
}
void MainWindow::setupUI()
{
    ui->setupUi(this);

    mpDialog = new ImagePaletDialog(this, mpAnimationModel);

    mpAnimationViewer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->animationViewer->addWidget(mpAnimationViewer);

    mpQTimelinePanel = new QTimelinePanel(mpAnimationModel, this);
    mpQTimelinePanel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->timelineContainer->addWidget(mpQTimelinePanel);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ResourceManager::loadWorkingDirectory();
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

void MainWindow::onSelectionChanged(const QItemSelection& item1, const QItemSelection& item2)
{
    QModelIndexList indexes = item1.indexes();
    QModelIndex index = indexes.takeFirst();

    QString path = mAnimationTreeViewModel.filePath(index);
    QFileInfo fileInfo = QFileInfo (path);
    if (fileInfo.isFile())
    {
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
        QModelIndex index = indexes.takeFirst();

        QString path = mAnimationTreeViewModel.filePath(index);

        QFile::remove(path);
        refreshTree();
    }
}

void MainWindow::onPaletButtonClicked()
{
   if (mpDialog->isHidden())
   {
       mpDialog->move(this->x(), this->y());
       mpDialog->show();
    }
}

void MainWindow::refreshTree()
{
    mAnimationTreeViewModel.refresh(QModelIndex());
}

void MainWindow::setWorkingDirectory()
{
    QString newDirectory = QFileDialog::getExistingDirectory
                    (
                        this,
                        tr("Open"),
                        ""
                    );

    ResourceManager::setWorkingDirectory(newDirectory);

    QString rootPath = ResourceManager::getResourcePath(ANIMATION_DIR.path());
    ui->animationTreeView->setRootIndex(mAnimationTreeViewModel.index(rootPath) );
}
