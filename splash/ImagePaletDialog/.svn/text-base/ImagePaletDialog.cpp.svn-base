#include "imagepaletdialog.h"
#include "ui_imagepaletdialog.h"
#include "DataModels/AnimationModel.h"
#include "ImagePaletDialog/AnimationImagePaletPanel.h"
#include "ResourceManager.h"
#include <QStandardItemModel>
#include <QString>

ImagePaletDialog::ImagePaletDialog(QWidget *parent, AnimationModel* pAnimationModel)
    : QDialog(parent),
    m_ui(new Ui::ImagePaletDialog),
    mpAnimationModel(pAnimationModel)
{
    m_ui->setupUi(this);
    mpAnimationImagePaletPanel = new AnimationImagePaletPanel(pAnimationModel);
    mpAnimationImagePaletPanel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_ui->scrollArea->setWidget(mpAnimationImagePaletPanel);

    QString rootPath = ResourceManager::getResourcePath("");

    mSourceFileTreeViewModel.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    mSourceFileTreeViewModel.setReadOnly(true);

    m_ui->fileSelectionTreeView->setModel(&mSourceFileTreeViewModel);
    m_ui->fileSelectionTreeView->setRootIndex(mSourceFileTreeViewModel.index(rootPath) );
    m_ui->fileSelectionTreeView->setAutoScroll(true);

    connect(m_ui->fileSelectionTreeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
            this, SLOT(onSelectionChanged(QItemSelection, QItemSelection)));
    connect(m_ui->spinBoxGridX, SIGNAL(valueChanged(int)), this, SLOT(snapGridChanged()));
    connect(m_ui->spinBoxGridY, SIGNAL(valueChanged(int)), this, SLOT(snapGridChanged()));
    connect(m_ui->snapGridCheckBox, SIGNAL(toggled(bool)), this, SLOT(snapGridChanged()));
    
    connect(mpAnimationModel, SIGNAL(selectedPaletChanged(QString)), this, SLOT(onAnimationImagePaletChanged(QString)));
    snapGridChanged();
}

ImagePaletDialog::~ImagePaletDialog()
{
    delete m_ui;
}

void ImagePaletDialog::showEvent(QShowEvent *event)
{
    mSourceFileTreeViewModel.refresh(QModelIndex());
}


void ImagePaletDialog::onSelectionChanged(const QItemSelection& item1, const QItemSelection& item2)
{
    QModelIndexList indexes = item1.indexes();
    QModelIndex index = indexes.takeFirst();

    QString path = mSourceFileTreeViewModel.filePath(index);
    QString rootPath = ResourceManager::getResourcePath("");
    rootPath.append("/");

    // we don't want to store absolute path
    path.replace(rootPath, "", Qt::CaseInsensitive);

    mpAnimationModel->setSelectedSourcePath(path);
}

void ImagePaletDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ImagePaletDialog::snapGridChanged()
{
    mpAnimationImagePaletPanel->setSnapGrid(m_ui->spinBoxGridX->value(), m_ui->spinBoxGridY->value(), m_ui->snapGridCheckBox->isChecked());
}

void ImagePaletDialog::onAnimationImagePaletChanged(QString path)
{

}
