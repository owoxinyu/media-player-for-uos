#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QWidget>
#include <QApplication>
#include <QList>
#include <QUrl>
#include <QFile>

namespace Ui {

class PlayList;
}

class PlayList : public QWidget
{
    Q_OBJECT

public:
    explicit PlayList(QWidget *parent = nullptr);
    ~PlayList();

    QUrl current();
    void add(QUrl url);
    QUrl remove();

    int count() {
        return urls.count();
    }

public slots:
    void set_loop(bool v) {
        this->loop = v;
    }
    void set_shuffle(bool v) {
        this->shuffle = v;
    }
    void next();
    void prev();
    void change_theme(int idx) {
        QString filepath;
        switch(idx) {
        case 0:
            filepath = ":/theme/white/playlist.qss";
            break;
        case 1:
            filepath = ":/theme/dark/playlist.qss";
            break;
        default:
            QColor themeColor = QApplication::palette().highlight().color();
            if (themeColor.lightness() > 127)
                filepath = ":/theme/white/playlist.qss";
            else
                filepath = ":/theme/dark/playlist.qss";
        }

        QFile qssfile(filepath);
        if (qssfile.open(QFile::ReadOnly)) {
            QString s = QLatin1String(qssfile.readAll());
            setStyleSheet(s);
        }
    }

signals:
    void selectedChanged(QUrl);

private:
    Ui::PlayList *ui;
    QList<QUrl> urls;
    bool loop = false;
    bool shuffle = false;
};

#endif // PLAYLIST_H
