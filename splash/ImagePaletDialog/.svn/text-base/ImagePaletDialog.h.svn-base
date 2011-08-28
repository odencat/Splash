#ifndef IMAGEPALETDIALOG_H
#define IMAGEPALETDIALOG_H

#include <QtGui/QDialog>

#include "SourceFileDirModel.h"
namespace Ui {
    class ImagePaletDialog;
}

class AnimationModel;
class AnimationImagePaletPanel;
class QItemSelection;
class QStandardItemModel;
class ImagePaletDialog : public QDialog {
    Q_OBJECT
public:
    ImagePaletDialog(QWidget *parent, AnimationModel* pAnimationModel);
    ~ImagePaletDialog();

private slots:
    void snapGridChanged();
    void onAnimationImagePaletChanged(QString path);
    void onSelectionChanged(const QItemSelection& item1, const QItemSelection& item2);
protected:
    void changeEvent(QEvent *e);
    void showEvent(QShowEvent *event);

private:

    Ui::ImagePaletDialog *m_ui;
    AnimationModel* mpAnimationModel;
    AnimationImagePaletPanel* mpAnimationImagePaletPanel;
    SourceFileDirModel mSourceFileTreeViewModel;
};

#endif // IMAGEPALETDIALOG_H
