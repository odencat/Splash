#include "QTimelineWidget.h"
#include <QHeaderView>
#include <QList>
#include <QItemSelection>
#include <QLayout>
#include <QMenu>
#include <QKeyEvent>
#include "DataModels/AnimationModel.h"

#include "DataModels/KeyFrameData.h"

#define EXTRA_FRAMES 500
#define BLANK_BRUSH QBrush();

static QTimelineWidget::CellType sKeyFrameSymbol[QTimelineWidget::KeyFrameState_COUNT][KeyFrame::KeyFrameType_COUNT] =
{
    {
        QTimelineWidget::CellType_emptyKey,
        QTimelineWidget::CellType_key,
        QTimelineWidget::CellType_key
    },
    {
        QTimelineWidget::CellType_emptyFrame,
        QTimelineWidget::CellType_fixFrame,
        QTimelineWidget::cellType_line,
    },
    {
        QTimelineWidget::CellType_emptyFrameEnd,
        QTimelineWidget::CellType_fixFrameEnd,
        QTimelineWidget::cellType_arrow
    }
};

static QString sBrushesFileNames[2][QTimelineWidget::CellType_COUNT] =
{
    {
        ":/icons/blank.png",
        ":/icons/line.png",
        ":/icons/empty_key.png",
        ":/icons/connect_key.png",
        ":/icons/arrow.png",
        ":/icons/fix_frame.png",
        ":/icons/fix_frame_end.png",
        ":/icons/empty_frame.png",
        ":/icons/empty_frame_end.png",
    },
    {
        ":/icons/blank_selected.png",
        ":/icons/line_selected.png",
        ":/icons/empty_key_selected.png",
        ":/icons/connect_key_selected.png",
        ":/icons/arrow_selected.png",
        ":/icons/fix_frame_selected.png",
        ":/icons/fix_frame_end_selected.png",
        ":/icons/empty_frame_selected.png",
        ":/icons/empty_frame_end_selected.png",
    }
};

static QBrush sBrushes[2][QTimelineWidget::CellType_COUNT];

QTimelineWidget::QTimelineWidget(AnimationModel* pAnimationModel, QWidget *parent)
:    QTableWidget(parent),
     mpAnimationModel(pAnimationModel),
     mSelectedCelPosition(QPoint(-1, -1)),
     mSelectedRow(-1),
     mDragging(false),
     mpCopyKeyFrameData(NULL)
{
    mKeyFramePosition.mFrameNo = -1;
    mKeyFramePosition.mLineNo = -1;

    for (int i = 0; i < 2; i++)
    {
     for (int j = 0; j < CellType_COUNT; j++)
     {
        sBrushes[i][j] = QBrush(QPixmap(sBrushesFileNames[i][j]));
     }
    }

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setMinimumHeight(200);
    this->horizontalHeader()->setStretchLastSection(false);
    this->horizontalHeader()->setDefaultSectionSize(10);

    this->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    this->horizontalHeader()->setVisible(false);

    this->verticalHeader()->setStretchLastSection(false);
    this->verticalHeader()->setDefaultSectionSize(20);

    this->verticalHeader()->setResizeMode(QHeaderView::Fixed);

    this->setAlternatingRowColors(true);
    this->setColumnCount(100);

    this->setRowCount(AnimationModel::LINE_COUNT + 1);

    this->setEditTriggers(0);
    this->setSelectionMode(QAbstractItemView::NoSelection);

    this->setFocusPolicy(Qt::NoFocus);

    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(contextMenu(const QPoint &)));
    this->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(onCellDoubleClicked(int, int)));
    connect(this, SIGNAL(cellPressed(int, int)), this, SLOT(onCellPressed(int, int)));
    connect(this, SIGNAL(cellEntered(int, int)), this, SLOT(onCellEntered(int, int)));

    connect(mpAnimationModel, SIGNAL(selectedKeyFramePositionChanged(int, int)), this, SLOT(onSelectedCellChanged(int, int)));
    connect(mpAnimationModel, SIGNAL(refreshTimeLine()), this, SLOT(refreshTimeLine()));
}

QTimelineWidget::~QTimelineWidget()
{
    delete mpCopyKeyFrameData;
}

void QTimelineWidget::contextMenu(const QPoint &pos) {
    QMenu *menu = new QMenu;
    menu->addAction(tr("&Copy"), this, SLOT(copyFrame()));
    menu->addAction(tr("&Paste"), this, SLOT(pasteFrame()));
    menu->addAction(tr("&Inert Empty Keyframe"), this, SLOT(insertEmptyFrame()));
    menu->addAction(tr("&Add Length"), this, SLOT(addLength()));
    menu->addAction(tr("&Reduce Length"), this, SLOT(reduceFrameLength()));
    menu->addAction(tr("&Delete Frame"), this, SLOT(deleteFrame()));

    menu->exec(this->mapToGlobal(pos));
}

void QTimelineWidget::copyFrame()
{
    KeyFrame::KeyFramePosition currentPosition = mpAnimationModel->getCurrentKeyFramePosition();

    if (currentPosition.mLineNo == AnimationModel::LINE_COUNT )
    {
        mCopyEventList = mpAnimationModel->getEventList(currentPosition.mFrameNo);
    }
    else
    {
        KeyFrame* pKeyFrame = mpAnimationModel->getKeyFrame(currentPosition.mLineNo, currentPosition.mFrameNo);

        if (pKeyFrame && pKeyFrame->mpKeyFrameData)
        {
            delete mpCopyKeyFrameData;
            mpCopyKeyFrameData = new KeyFrameData(*pKeyFrame->mpKeyFrameData);
        }
    }
}

void QTimelineWidget::pasteFrame()
{   KeyFrame::KeyFramePosition currentPosition = mpAnimationModel->getCurrentKeyFramePosition();
    if (currentPosition.mLineNo == AnimationModel::LINE_COUNT )
    {
        mpAnimationModel->setEventList(currentPosition.mFrameNo, mCopyEventList);
    }
    else
    {
        if (mpCopyKeyFrameData)
        {
            mpAnimationModel->setKeyFrame(currentPosition.mLineNo, currentPosition.mFrameNo, new KeyFrameData(*mpCopyKeyFrameData));
        }
    }
}

void QTimelineWidget::insertEmptyFrame()
{
    KeyFrame::KeyFramePosition currentPosition = mpAnimationModel->getCurrentKeyFramePosition();
    mpAnimationModel->insertEmptyKeyFrame(currentPosition.mLineNo, currentPosition.mFrameNo);
}

void QTimelineWidget::addLength()
{
    KeyFrame::KeyFramePosition currentPosition = mpAnimationModel->getCurrentKeyFramePosition();
    mpAnimationModel->addFrameLength(currentPosition.mLineNo, currentPosition.mFrameNo, 1);
}

void QTimelineWidget::deleteFrame()
{
    KeyFrame::KeyFramePosition currentPosition = mpAnimationModel->getCurrentKeyFramePosition();
    mpAnimationModel->clearFrames(currentPosition.mLineNo, currentPosition.mFrameNo, currentPosition.mFrameNo);
}

void QTimelineWidget::reduceFrameLength()
{
    KeyFrame::KeyFramePosition currentPosition = mpAnimationModel->getCurrentKeyFramePosition();
    mpAnimationModel->reduceFrameLength(currentPosition.mLineNo, currentPosition.mFrameNo);
}


void QTimelineWidget::keyPressEvent(QKeyEvent *event)
{
    KeyFrame::KeyFramePosition currentPosition = mpAnimationModel->getCurrentKeyFramePosition();
    switch(event->key())
    {
        case Qt::Key_A:
            addLength();
        break;
        case Qt::Key_C:
            copyFrame();
        break;
        case Qt::Key_D:
            deleteFrame();
        break;
        case Qt::Key_V:
            pasteFrame();
        break;
        case Qt::Key_R:
            reduceFrameLength();
        break;

        // Insert Empty key frame
        case Qt::Key_E:
            insertEmptyFrame();
        break;

        case Qt::Key_Delete:
            mpAnimationModel->clearFrames(currentPosition.mLineNo, currentPosition.mFrameNo, currentPosition.mFrameNo);
        break;
    }

    QTableWidget::keyPressEvent(event);
}

// Refresh visible timeline area
void QTimelineWidget::refreshTimeLine()
{
    this->setColumnCount(mpAnimationModel->getMaxFrameCount() + EXTRA_FRAMES);
    clearCells();
    this->clearSelection();

    QRect visibleRect = this->visibleRegion().boundingRect();
    int leftColumn = this->columnAt(visibleRect.left()) - 1;
    int rightColumn = this->columnAt(visibleRect.right());

    for (int lineNo = 0; lineNo < AnimationModel::LINE_COUNT; lineNo++)
    {
        const QList<KeyFrame*>& keyframeList = mpAnimationModel->getKeyFrameList(lineNo);

        for (int keyframeIndex = 0;  keyframeIndex < keyframeList.count(); keyframeIndex++)
        {
            int startFrameNo = keyframeList[keyframeIndex]->mFrameNo;
            if (startFrameNo > rightColumn) {break;}

            int endFrameNo = startFrameNo;
            if (keyframeIndex + 1 < keyframeList.count())
            {
                endFrameNo = keyframeList[keyframeIndex + 1]->mFrameNo - 1;
            }

            KeyFrame::KeyFrameType keyframeType = keyframeList[keyframeIndex]->getKeyFrameType();

            // Find out whether any of attribute is tweened
            for (int i = 0; i < KeyFrameData::TweenAttribute_COUNT; i++)
            {
                int prevIndex = mpAnimationModel->getPreviousKeyFrameIndex(lineNo, startFrameNo, KeyFrameData::TweenAttribute(i));
                if (prevIndex >= 0)
                {
                    if(keyframeList[prevIndex]->getKeyFrameType() == KeyFrame::KeyFrameType_tween)
                    {
                       keyframeType = KeyFrame::KeyFrameType_tween;
                    }
                }
            }

            // Draw on sequence of tween frames
            if (startFrameNo <= rightColumn)
            {
                int visibleStartFrameNo = std::max(startFrameNo, leftColumn);
                int visibleEndFrameNo = std::min(endFrameNo, rightColumn);

                // set start of tween / fix
                if (visibleStartFrameNo == startFrameNo)
                {
                    setCellItem(lineNo, startFrameNo, sKeyFrameSymbol[KeyFrameState_start][keyframeType]);
                }

                // set items between start and end
                for (int frameNo = visibleStartFrameNo + 1; frameNo < visibleEndFrameNo; frameNo++)
                {
                    setCellItem(lineNo, frameNo, sKeyFrameSymbol[KeyFrameState_between][keyframeType]);
                }

                // set end of tween / fix
                if (visibleEndFrameNo != startFrameNo && visibleEndFrameNo == endFrameNo)
                {
                    setCellItem(lineNo, endFrameNo, sKeyFrameSymbol[KeyFrameState_end][keyframeType]);
                }
            }
        }
    }

    // Refresh Event key
    for (int i = leftColumn; i <= rightColumn; i++)
    {
        AnimationModel::EventList list = mpAnimationModel->getEventList(i);
        if (list.mList.count() > 0)
        {
            setCellItem(AnimationModel::LINE_COUNT, i, CellType_key);
        }
    }

    KeyFrame::KeyFramePosition keyframePosition = mpAnimationModel->getCurrentKeyFramePosition();
    setCellSelection(keyframePosition.mLineNo, keyframePosition.mFrameNo, true);
}
 void QTimelineWidget::clearCells()
 {
    this->clear();
 }

 void QTimelineWidget::clearSelections()
 {
    QRect visibleRect = this->visibleRegion().boundingRect();
    int leftColumn = this->columnAt(visibleRect.left()) - 1;
    int rightColumn = this->columnAt(visibleRect.right());

    for (int i = 0; i < this->rowCount(); i++)
    {
        for (int j = leftColumn; j < rightColumn; j++)
        {
            setCellSelection(i, j, false);
        }
    }
 }

 void QTimelineWidget::setCellItem(int row, int column, CellType cellType)
 {
     if (row <0 || column < 0){return;}
     if (!this->item(row, column)){
        this->setItem(row, column, new QTableWidgetItem(QString(""), Qt::DisplayRole));
     }

     this->item(row, column)->setBackground(sBrushes[false][cellType]);
 }

 void QTimelineWidget::setCellSelection(int row, int column, bool selection)
 {
     if (row <0 || column < 0){return;}
     if (!this->item(row, column)){
        if (!selection)
        {
            // if no widget item exists and if there is no selection don't do anything here
            return;
        }
        this->setItem(row, column, new QTableWidgetItem(QString(""), Qt::DisplayRole));
        this->item(row, column)->setBackground(sBrushes[0][0]);
     }

     const QBrush& selectedBrush = this->item(row, column)->background();
     CellType cellType = CellType_blank;
     bool selected = false;
     for (int i = 0; i < 2; i++)
     {
         for (int j = 0; j < CellType_COUNT; j++)
         {
            if (selectedBrush == sBrushes[i][j])
            {
                selected = i;
                cellType = (CellType)j;
                break;
            }
         }
     }

     if (!selection && cellType  == CellType_blank)
     {
        // if it is blank, release the memory for the widget
        this->item(row, column)->setBackground(sBrushes[selection][cellType]);
        delete this->item(row, column);
     }
     else
     {
         this->item(row, column)->setBackground(sBrushes[selection][cellType]);
     }

     this->setVerticalHeaderItem(AnimationModel::LINE_target, new QTableWidgetItem(QString("T")));
     this->setVerticalHeaderItem(AnimationModel::LINE_camera, new QTableWidgetItem(QString("C")));
     this->setVerticalHeaderItem(AnimationModel::LINE_COUNT, new QTableWidgetItem(QString("E")));
 }

 void QTimelineWidget::setKeyFrame(int row, int column)
 {

 }

 void QTimelineWidget::removeKeyFrame(int row, int column)
 {

 }

 void QTimelineWidget::setFrameCount(int row, int count)
 {

 }

 void QTimelineWidget::onSelectedCellChanged(int row, int column)
 {
      clearSelections();
      setCellSelection(row, column, true);
 }

 void QTimelineWidget::onCellDoubleClicked(int row, int column)
 {

 }

 void QTimelineWidget::onCellPressed(int row, int column)
 {
    this->setFocus();
    KeyFrame::KeyFramePosition keyframePosition = mpAnimationModel->getCurrentKeyFramePosition();

    if (keyframePosition.mLineNo != row || keyframePosition.mFrameNo != column)
    {
        clearSelections();

        // TODO: Moving key frame feature
        //mDragging = true;

        mKeyFramePosition.mLineNo = row;
        mKeyFramePosition.mFrameNo = column;

        setCursor(Qt::ArrowCursor);
        mSelectedCelPosition = QPoint(row, column);
        mSelectedRow = row;

        mpAnimationModel->selectCurrentKeyFramePosition(row, column);
    }
 }

 void QTimelineWidget::onCellEntered(int row, int column)
 {
// TODO: Moving key frame feature
//    if (mDragging)
//    {
//        if (row != mSelectedRow)
//        {
//            setCursor(Qt::ForbiddenCursor);
//            row = mSelectedRow;
//        }
//        else
//        {
//            setCursor(Qt::ArrowCursor);
//            mSelectedCelPosition = QPoint(row, column);
//        }
//        setCellSelection(row, column, true);
//    }

 }

 void QTimelineWidget::mouseReleaseEvent(QMouseEvent * event)
 {
    mDragging = false;
    setCursor(Qt::ArrowCursor);
 }
