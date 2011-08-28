#include "QTimelinePanel.h"
#include "ui_QTimelinePanel.h"
#include "QTimelineWidget.h"
#include "QScrollbar.h"
#include "DataModels/AnimationModel.h"

QTimelinePanel::QTimelinePanel(AnimationModel* pAnimationModel, QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::QTimelinePanel),
    mpQTimelineWidget(NULL)
{
    m_ui->setupUi(this);

    mpQTimelineWidget = new QTimelineWidget(pAnimationModel, this);
    m_ui->timelineWidgetContainer->addWidget(mpQTimelineWidget);

    connect(mpQTimelineWidget->horizontalScrollBar(), SIGNAL(valueChanged (int)), this, SLOT(onTimelineScrolled(int)));
}

QTimelinePanel::~QTimelinePanel()
{
    delete m_ui;
}

void QTimelinePanel::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void QTimelinePanel::onTimelineScrolled(int value)
{
    //m_ui->currentFrameNo->setText(QString("%0").arg(value));
    mpQTimelineWidget->refreshTimeLine();
}
