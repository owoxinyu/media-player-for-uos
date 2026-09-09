#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QAudioOutput>
#include <QWindow>
#include <QResizeEvent>
#include <QMenu>
#include <QFile>
#include	<QToolBar>
#include	<QDockWidget>
#include "./playbar.h"
#include "./playlist.h"
#include "./setting.h"
#include "./shortcutkeyedititem.h"
#include "./modelchat.h"
#include <QPalette>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void change_theme(int idx) {
        QString filepath;
        switch(idx) {
        case 0:
            filepath = ":/theme/white/mainwindow.qss";
            break;
        case 1:
            filepath = ":/theme/dark/mainwindow.qss";
            break;
        default:
            QColor themeColor = QApplication::palette().highlight().color();
            qDebug() << "themecolor-lightness=" << themeColor.lightness();
            if (themeColor.lightness() > 127)
                filepath = ":/theme/white/mainwindow.qss";
            else
                filepath = ":/theme/dark/mainwindow.qss";
        }

        QFile qssfile(filepath);
        if (qssfile.open(QFile::ReadOnly)) {
            QString s = QLatin1String(qssfile.readAll());
            setStyleSheet(s);
        }
    }


    void ffmpeg_video_cut() {
        double start = media_player.position();
        double duration = 3000;
        if (start + duration - 1000> media_player.duration())
            duration = fmax(media_player.duration() - start, 1000);
        if (start > 1000)
            start -= 1000;
        else
            start = 0;
        _ffmpeg_video_cut(media_player.source().toString(), "/tmp/.video_piece.mp4", start / 1000, duration / 1000);
    }

    void _ffmpeg_video_cut(QString in, QString out, int start, int duration) {
        QString prog = "/usr/bin/ffmpeg";
        QStringList args_;
        args_ << "-ss" << QString::number(start) << "-t" << QString::number(duration) << "-i" << in << "-c" << "copy" << "-y" << out;
        QProcess *process = new QProcess();
        process->start(prog, args_);
        connect(process, &QProcess::finished, [=](int err, QProcess::ExitStatus status) {
            qDebug() << in << " ffmpeged to " << out;
            if (!err)
                emit ffmpeg_video_cut_ok();
            else
                emit ffmpeg_video_cut_err();
        });
    }

signals:
    void ffmpeg_video_cut_ok();
    void ffmpeg_video_cut_err();


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


    QMediaPlayer media_player;
    QVideoWidget video_output;
    QAudioOutput audio_output;
    PlayBar player_bar;
    // QToolBar tool_bar;
    QDockWidget tool_bar;
    PlayList playlist;
    Setting setting;
    QPalette palette;
    ModelChat modelchat;


protected:
    void mouseMoveEvent(QMouseEvent *);


private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
