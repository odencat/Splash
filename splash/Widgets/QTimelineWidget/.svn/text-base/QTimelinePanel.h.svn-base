#ifndef QTIMELINEPANEL_H
#define QTIMELINEPANEL_H

#include <QtGui/QWidget>

namespace Ui {
    class QTimelinePanel;
}

class AnimationModel;
class QTimelineWidget;
class QTimelinePanel : public QWidget {
    Q_OBJECT
public:
    QTimelinePanel(AnimationModel* pAnimationModel, QWidget *parent = 0);
    ~QTimelinePanel();

protected:
    void changeEvent(QEvent *e);

private slots:
    void onTimelineScrolled(int value);

private:
    Ui::QTimelinePanel *m_ui;
    QTimelineWidget* mpQTimelineWidget;
};

#endif // QTIMELINEPANEL_H
