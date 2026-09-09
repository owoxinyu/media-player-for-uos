#ifndef GRABLEPROGRESSBAR_H
#define GRABLEPROGRESSBAR_H

#include <QProgressBar>
#include <QMouseEvent>

class GrableProgressBar : public QProgressBar
{
    Q_OBJECT
public:
    explicit GrableProgressBar(QWidget *parent = nullptr);
protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
signals:
    void userChangedValue(double value);
};

#endif // GRABLEPROGRESSBAR_H
