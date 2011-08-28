#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "DataModels/AnimationModel.h"
#include "QDir.h"
#include "SourceFileDirModel.h"

class AnimationViewer;
class ImagePaletDialog;
class ResourceTree;
class QSignalMapper;
class QStandardItemModel;
class QItemSelection;
class QTimelinePanel;
namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setEditEnabled(bool enabled);

public slots:
    void setEnabled(bool enabled);
    void refreshTree();
    void refreshEventList();

private slots:
    // animation list control
    void onAddEventButtonClicked();
    void onRemoveEventButtonClicked();
    void onEventEdited(int row, int column);

    void onAddAnimationButtonClicked();
    void onRemoveAnimationButtonClicked();

    void onPaletButtonClicked();
    
    void onAnimationSelected(int index);

    // Animation Panel Control

    void onSelectionChanged(const QItemSelection& item1, const QItemSelection& item2);

    void setWorkingDirectory();

protected:
private:
    void setupConnections();
    void setupModels();
    void setupUIModels();
    void setupUI();
    void loadAnimationData(int index);


    Ui::MainWindow *ui;

    QStandardItemModel *mpAnimationListModel;

    AnimationModel* mpAnimationModel;
    AnimationViewer* mpAnimationViewer;
    QTimelinePanel* mpQTimelinePanel;

    SourceFileDirModel mAnimationTreeViewModel;

    ImagePaletDialog *mpDialog;

    ResourceTree *mpResourceTree;
};

#endif // MAINWINDOW_H
