#include "playbar.h"
#include "ui_playbar.h"

#include <QTimer>
#include <QIcon>
#include <QFile>
#include <QLineEdit>

PlayBar::PlayBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayBar)
{
    ui->setupUi(this);

    volume = ui->volumeSlider->value();

    // QTimer *qsstimer = new QTimer();
    // qsstimer->setInterval(1000);
    // qsstimer->setSingleShot(false);
    // ui->speedSpin->setValue(1);
    // connect(qsstimer, &QTimer::timeout, [=]() {
        // QString filepath("./qss/dark/%1.qss");
        // filepath = filepath.arg("playbar");

        // QFile qssfile(filepath);
        // if (qssfile.open(QFile::ReadOnly)) {
            // QString s = QLatin1String(qssfile.readAll());
            // this->setStyleSheet(s);
            // qDebug() << filepath << ": updated Style";
        // } else {
            // if (qssfile.open(QFile::WriteOnly))
                // qssfile.close();
            // qDebug() << filepath << "failed to update Style";
        // }
    // });
    // qsstimer->start();

    // ui->volumeLabel->setText(QString::asprintf("%d%%", ui->volumeSlider->value()));

    connect(this, &PlayBar::playChanged, [=](bool v) {
        ui->playButton->setIcon(v ? QIcon(":/images/pause.svg") : QIcon(":/images/play.svg"));
    });

    connect(this, &PlayBar::volumeChanged, ui->volumeSlider, &QSlider::setValue);
    connect(ui->volumeSlider, &QSlider::valueChanged, [=](int value) {
        double pos = (double)value / (double)(ui->volumeSlider->maximum() - ui->volumeSlider->minimum());
        qDebug() << "pos=" << pos;
        if (pos == 0)
            ui->soundButton->setIcon(QIcon(":/images/sound_mute.svg"));
        else if (pos < 0.25)
            ui->soundButton->setIcon(QIcon(":/images/sound_0.svg"));
        else if (pos < 0.50)
            ui->soundButton->setIcon(QIcon(":/images/sound_1.svg"));
        else if (pos < 0.75)
            ui->soundButton->setIcon(QIcon(":/images/sound_2.svg"));
        else
            ui->soundButton->setIcon(QIcon(":/images/sound_3.svg"));
        // ui->volumeLabel->setText(QString::asprintf("%d%%", ui->volumeSlider->value()));
    });

    connect(this, &PlayBar::postionChanged, [=](qint64 pos) {
        setTimeLabel(ui->playedProgressLabel, pos / 1000);
        // ui->playProgressBar->setValue((int)((double)postion / (double)duration * 100));
        ui->progressSlider->setValue(postion);
    });

    connect(this, &PlayBar::durationChanged, [=](qint64 dur) {
        // ui->playProgressBar->setValue(0);
        ui->progressSlider->setValue(0);
        ui->progressSlider->setMaximum(dur);
        setTimeLabel(ui->totalProgressLabel, duration / 1000);
    });

    ui->speedSpin->setValue(1.0);
}

PlayBar::~PlayBar()
{
    delete ui;
}

void PlayBar::setVolume(int value) {
    if (ui->volumeSlider->value() == value)
        return;
    ui->volumeSlider->setValue(value);
}

void PlayBar::volumeIncreace(int value) {
    this->setVolume(ui->volumeSlider->value() + value);
}

void PlayBar::volumeDecreace(int value) {
    this->setVolume(ui->volumeSlider->value() - value);
}

void PlayBar::setPosition(qint64 pos) {
    this->postion = pos;
    emit postionChanged(pos);
}

void PlayBar::setDuration(qint64 duration) {
    this->duration = duration;
    emit durationChanged(duration);
}

void PlayBar::toggle_play() {
    ui->playButton->click();
}

void PlayBar::toggle_playlist() {
    ui->playlistButton->click();
}

void PlayBar::toggle_mute() {
    ui->soundButton->click();
}

void PlayBar::toggle_random() {
    ui->randomButton->toggle();
}
void PlayBar::toggle_loop() {
    ui->repeatButton->toggle();
}

void PlayBar::on_volumeSlider_valueChanged(int value) {
    if (value == ui->volumeSlider->minimum())
        ui->soundButton->setChecked(true);
    else {
        ui->soundButton->setChecked(false);
        volume = value;
    }
    emit volumeChanged(value);
}

void PlayBar::on_randomButton_clicked() {
    qDebug() << "random=" << ui->randomButton->isChecked();
    emit randomChanged(ui->randomButton->isChecked());
}
void PlayBar::on_repeatButton_clicked() {
    qDebug() << "repeat=" << ui->repeatButton->isChecked();
    emit loopChanged(ui->repeatButton->isChecked());
}


void PlayBar::source_setted(bool v) {
    if (playing)
        ui->playButton->click();
    have_source = v;
    if (!v)
        setPosition(0);
}

void PlayBar::on_playlistButton_clicked() {
    emit playlistChanged(ui->playlistButton->isChecked());
}

void PlayBar::on_soundButton_clicked() {
    int n_volume = ui->soundButton->isChecked() ? 0 : volume;
    this->setVolume(n_volume);
}
