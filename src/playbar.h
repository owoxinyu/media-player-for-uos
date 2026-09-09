#ifndef PLAYBAR_H
#define PLAYBAR_H

#include <QWidget>
#include <QApplication>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QProgressBar>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QResizeEvent>
#include <QFile>
#include <QMediaMetaData>
#include "./mytimeformater.h"
#include "./playlist.h"
#include <QDebug>


namespace Ui {
class PlayBar;
}


class PlayBar : public QWidget
{
    Q_OBJECT

public:
    explicit PlayBar(QWidget *parent = nullptr);
    ~PlayBar();

    void setTimeLabel(QLabel *label, double time) {
        myTimeFormater formater(time);
        label->setText(QString::asprintf("%02d:%02d:%02d", formater.hour, formater.minute, (int)formater.second));
    }

    void setVolume(int value);

public slots:
    void setPosition(qint64);
    void setDuration(qint64);
    void volumeIncreace(int value);
    void volumeDecreace(int value);
    void toggle_mute();
    void toggle_play();
    void toggle_playlist();
    void toggle_random();
    void toggle_loop();
    void source_setted(bool);
    void on_volumeSlider_valueChanged(int);
    void on_soundButton_clicked();
    void on_playlistButton_clicked();
    void on_repeatButton_clicked();
    void on_randomButton_clicked();
    void on_feelingButton_clicked() {
        emit feelingClicked();
    }
    void change_theme(int idx) {
        QString filepath;
        switch(idx) {
        case 0:
            filepath = ":/theme/white/playbar.qss";
            break;
        case 1:
            filepath = ":/theme/dark/playbar.qss";
            break;
        default:
            QColor themeColor = QApplication::palette().highlight().color();
            if (themeColor.lightness() > 127)
                filepath = ":/theme/white/playbar.qss";
            else
                filepath = ":/theme/dark/playbar.qss";
        }

        QFile qssfile(filepath);
        if (qssfile.open(QFile::ReadOnly)) {
            QString s = QLatin1String(qssfile.readAll());
            setStyleSheet(s);
        }
    }
protected:
    qint64 postion, duration;

private slots:

    void on_volumeSlider_sliderPressed() {
    }

    // void on_zoomButton_clicked() {
        // emit zoomClicked();
    // }

    void on_playButton_clicked() {
        if (!have_source)
            return;
        playing = !playing;
        emit playChanged(playing);
    }
    // void on_stopButton_clicked() {
        // emit stopClicked();
    // }
    void on_prevButton_clicked() {
        emit prevClicked();
    }
    void on_nextButton_clicked() {
        emit nextClicked();
    }
    void on_settingButton_clicked() {
        emit settingClicked();
    }

    // void on_playProgressBar_userChangedValue(double value) {
        // emit userChangedProgress(value);
    // }

    void on_progressSlider_sliderPressed() {
    }

    void on_progressSlider_sliderMoved(int value) {
        emit userChangedProgress(value);
    }

    void on_speedSpin_valueChanged(double value) {
        emit speedChanged(value);
    }



signals:
    void volumeChanged(int value);
    void playlistChanged(bool v);
    void zoomClicked();
    void playChanged(bool v);
    void stopClicked();
    void prevClicked();
    void nextClicked();
    void feelingClicked();
    void settingClicked();
    void loopChanged(bool checked);
    void randomChanged(bool checked);
    void settingChanged(bool checked);
    void postionChanged(qint64);
    void durationChanged(qint64);
    void speedChanged(double);
    void userChangedProgress(int value);

private:
    int volume;
    bool playing = false, have_source = false, setting = false;

    Ui::PlayBar *ui;
};

#endif // PLAYBAR_H
