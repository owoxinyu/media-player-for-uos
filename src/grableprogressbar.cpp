#include "grableprogressbar.h"
#include	<QTimer>
#include	<QFile>

GrableProgressBar::GrableProgressBar(QWidget *parent):
    QProgressBar(parent)
{
    QTimer *qsstimer = new QTimer();
    qsstimer->setInterval(1000);
    qsstimer->setSingleShot(false);
    connect(qsstimer, &QTimer::timeout, [=]() {
        QString filepath("./qss/dark/%1.qss");
        filepath = filepath.arg("grableprogressbar");

        QFile qssfile(filepath);
        if (qssfile.open(QFile::ReadOnly)) {
            QString s = QLatin1String(qssfile.readAll());
            this->setStyleSheet(s);
            qDebug() << filepath << ": updated Style";
        } else {
            if (qssfile.open(QFile::WriteOnly))
                qssfile.close();
            qDebug() << filepath << "failed to update Style";
        }
    });
    // qsstimer->start();

    setMouseTracking(true);
}

void GrableProgressBar::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton)
        emit userChangedValue((double)event->pos().rx() / (double)width());
}

void GrableProgressBar::mousePressEvent(QMouseEvent *event) {
    emit userChangedValue((double)event->pos().rx() / (double)width());
}
