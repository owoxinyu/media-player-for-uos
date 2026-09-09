#ifndef MODELCHAT_H
#define MODELCHAT_H

#include <QWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QApplication>
#include <QFile>
#include <QTimer>
#include <QtWebSockets/QWebSocket>

namespace Ui {
class ModelChat;
}

class ModelChat : public QWidget
{
    Q_OBJECT

public:
    explicit ModelChat(QWidget *parent = nullptr);
    ~ModelChat();

public slots:
    void on_connectButton_clicked();
    void on_sendButton_clicked();
    void chat_start();
    void chat_fn(const QString &);
    void add_chatbox(QString msg, int role = 0);
    void change_theme(int idx) {
        QString filepath;
        switch(idx) {
        case 0:
            filepath = ":/theme/white/modelchat.qss";
            break;
        case 1:
            filepath = ":/theme/dark/modelchat.qss";
            break;
        default:
            QColor themeColor = QApplication::palette().highlight().color();
            qDebug() << "themecolor-lightness=" << themeColor.lightness();
            if (themeColor.lightness() > 127)
                filepath = ":/theme/white/modelchat.qss";
            else
                filepath = ":/theme/dark/modelchat.qss";
        }

        QFile qssfile(filepath);
        if (qssfile.open(QFile::ReadOnly)) {
            QString s = QLatin1String(qssfile.readAll());
            setStyleSheet(s);
        }
    }

signals:
    void video_cut_req();

private:
    Ui::ModelChat *ui;
    QWebSocket fd;
    QString session_hash = "hs5c4gzsesj";
    double processing_time = 0;
    QTimer *processing_timer = new QTimer();
};

#endif // MODELCHAT_H
