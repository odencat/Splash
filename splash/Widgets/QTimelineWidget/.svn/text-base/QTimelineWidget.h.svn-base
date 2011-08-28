#ifndef QTIMELINEWIDGET_H
#define QTIMELINEWIDGET_H

#include <QTableWidget>
#include <QPoint>
#include "DataModels/KeyFrame.h"
#include "DataModels/AnimationModel.h"

class KeyFrameData;
class QTimelineWidget : public QTableWidget
{
    Q_OBJECT
public:

     enum KeyFrameState
     {
         KeyFrameState_start,
         KeyFrameState_between,
         KeyFrameState_end,

         KeyFrameState_COUNT
     };

     enum CellType
     {
         CellType_blank,
         cellType_line,
         CellType_emptyKey,
         CellType_key,

         cellType_arrow,
         CellType_fixFrame,
         CellType_fixFrameEnd,
         CellType_emptyFrame,
         CellType_emptyFrameEnd,

         CellType_COUNT
     };

    QTimelineWidget(AnimationModel* pAnimationModel, QWidget *parent = 0);
    ~QTimelineWidget();

public slots:
 void refreshTimeLine();

 void onSelectedCellChanged(int row, int column);

protected:
 void keyPressEvent(QKeyEvent *event);
 void mouseReleaseEvent(QMouseEvent* event);

private slots:
 void clearCells();
 void clearSelections();
 void onCellDoubleClicked(int row, int column);
 void onCellPressed(int row, int column);
 void onCellEntered(int row, int column);
 void contextMenu(const QPoint &pos);
 void copyFrame();
 void deleteFrame();
 void pasteFrame();
 void insertEmptyFrame();
 void addLength();
 void reduceFrameLength();

private:
 void setKeyFrame(int row, int column);
 void removeKeyFrame(int row, int column);
 void setFrameCount(int row, int count);
 void setCellItem(int row, int column, CellType cellType);
 void setCellSelection(int row, int column, bool selection);

 AnimationModel* mpAnimationModel;
 QPoint mSelectedCelPosition;
 int mSelectedRow;
 bool mDragging;
 KeyFrameData* mpCopyKeyFrameData;
 AnimationModel::EventList mCopyEventList;
 KeyFrame::KeyFramePosition mKeyFramePosition;
};

#endif // QTIMELINEWIDGET_H
