#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include	<QAction>
#include	<QFileDialog>
#include	<QSizePolicy>
#include	<QMediaDevices>
#include	<QAudioDevice>
#include	<QTimer>
#include	<QFile>
#include	<QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // tool_bar.setWindowFlags(tool_bar.windowFlags() | Qt::WindowStaysOnTopHint);
    // playlist.setWindowFlags(playlist.windowFlags() | Qt::WindowStaysOnTopHint);
    // player_bar.setWindowFlags(player_bar.windowFlags() | Qt::WindowStaysOnTopHint);

    // QTimer *qsstimer = new QTimer();
    // qsstimer->setInterval(1000);
    // qsstimer->setSingleShot(false);
    // connect(qsstimer, &QTimer::timeout, [=]() {
        // QString filepath("./qss/dark/%1.qss");
        // filepath = filepath.arg("mainwindow");

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
        // // qDebug() << "Player Bar isVisible:" << player_bar.isVisible();
        // // qDebug() << "Player Bar geometry:" << player_bar.geometry();
        // // qDebug() << "Video Output geometry:" << video_output.geometry();
        // // const int margin = 15;  // 统一边距

        // // player_bar.setGeometry(
            // // margin,  // X：左侧留空
            // // video_output.height() - player_bar.height() - margin,  // Y：底部留空
            // // video_output.width() - 2 * margin,  // 宽度：左右扣除边距
            // // player_bar.height()  // 高度保持不变
            // // );
        // // player_bar.update();
        // player_bar.show();
    // });
    // qsstimer->start();

    ui->verticalLayout->addWidget(&video_output);
    ui->verticalLayout->addWidget(&player_bar);


    setting.addShortcut(new ShortcutKeyEditItem("播放", Qt::Key_Space, ui->actionPlay));
    setting.addShortcut(new ShortcutKeyEditItem("循环播放", Qt::Key_L, ui->actionLoopStatus));
    setting.addShortcut(new ShortcutKeyEditItem("随机播放", Qt::Key_R, ui->actionRandom));
    setting.addShortcut(new ShortcutKeyEditItem("播放列表", Qt::Key_P, ui->actionPlaylist));
    setting.addShortcut(new ShortcutKeyEditItem("全屏", Qt::Key_F11, ui->actionFullscreen));
    setting.addShortcut(new ShortcutKeyEditItem("上一首", Qt::Key_K, ui->actionPrevious));
    setting.addShortcut(new ShortcutKeyEditItem("下一首", Qt::Key_J, ui->actionNext));
    setting.addShortcut(new ShortcutKeyEditItem("增加音量", Qt::Key_Up, ui->actionIncreace_Volume));
    setting.addShortcut(new ShortcutKeyEditItem("减小音量", Qt::Key_Down, ui->actionDecrease_Volume));
    setting.addShortcut(new ShortcutKeyEditItem("静音", Qt::Key_M, ui->actionMute));
    setting.addShortcut(new ShortcutKeyEditItem("后进", Qt::Key_Left, ui->actionBackward));
    setting.addShortcut(new ShortcutKeyEditItem("前进", Qt::Key_Right, ui->actionForward));
    setting.addShortcut(new ShortcutKeyEditItem("内容感知", Qt::Key_F, ui->actionFeeling));

    QAction *seek_left = new QAction(), *seek_right = new QAction();
    connect(ui->actionBackward, &QAction::triggered, [&]() {
        media_player.setPosition(media_player.position() - 500);
    });
    connect(ui->actionForward, &QAction::triggered, [&]() {
        media_player.setPosition(media_player.position() + 500);
    });

    connect(&player_bar, &PlayBar::speedChanged, [&](double speed) {
        media_player.setPlaybackRate(speed);
    });


    video_output.setAttribute(Qt::WA_TransparentForMouseEvents);

    centralWidget()->setMouseTracking(true);
    setMouseTracking(true);

    // video_output.setParent(ui->widget);
    // player_bar.setParent(ui->widget);
    // video_output.setFixedSize(ui->widget->width(), ui->widget->height());
    // player_bar.setFixedSize(72, 422);

    // tool_bar.setAllowedAreas(Qt::RightToolBarArea);
    tool_bar.setAllowedAreas(Qt::RightDockWidgetArea);
    tool_bar.setSizePolicy(QSizePolicy::Policy::Preferred,
                           QSizePolicy::Policy::Fixed);

    media_player.setVideoOutput(&video_output);
    media_player.setAudioOutput(&audio_output);

    // this->addToolBar(Qt::RightToolBarArea, &tool_bar);

    this->addDockWidget(Qt::RightDockWidgetArea, &tool_bar);

    tool_bar.setTitleBarWidget(new QLabel("播放列表"));
    tool_bar.setFeatures(tool_bar.features()
                         &~ QDockWidget::DockWidgetFloatable
                         &~ QDockWidget::DockWidgetMovable
                         &~ QDockWidget::DockWidgetClosable
                         );
    // tool_bar.setMovable(false);
    // tool_bar.addWidget(&playlist);
    tool_bar.setWidget(&playlist);

    for (auto device: QMediaDevices::audioOutputs()) {
        QAction *action_device = new QAction(QString("%1")
                                             .arg(device.description()));
        ui->menuAudio_Device->addAction(action_device);
        connect(action_device, &QAction::triggered, [=]() {
            audio_output.setDevice(device);
        });
    }

    connect(ui->actionIncreace_Volume, &QAction::triggered, [&]() {
        player_bar.volumeIncreace(5);
    });
    connect(ui->actionDecrease_Volume, &QAction::triggered, [&]() {
        player_bar.volumeDecreace(5);
    });
    connect(ui->actionMute, &QAction::triggered, &player_bar, &PlayBar::toggle_mute);
    connect(ui->actionNext, &QAction::triggered, &playlist, &PlayList::next);
    connect(ui->actionPrevious, &QAction::triggered, &playlist, &PlayList::prev);

    connect(ui->actionLoopStatus, &QAction::triggered, &player_bar, &PlayBar::toggle_loop);
    connect(ui->actionRandom, &QAction::triggered, &player_bar, &PlayBar::toggle_random);

    connect(&player_bar, &PlayBar::userChangedProgress, [&](int value) {
        // media_player.setPosition(media_player.duration() * value);
        qDebug() << "userChangedProgress=" << value;
        media_player.setPosition(value);
    });

    connect(&media_player, &QMediaPlayer::mediaStatusChanged, [&]() {
        if (media_player.mediaStatus() != QMediaPlayer::MediaStatus::NoMedia)
            return;
        player_bar.setDuration(0);
    });
    connect(&media_player, &QMediaPlayer::mediaStatusChanged, [&]() {
        if (media_player.mediaStatus() != QMediaPlayer::MediaStatus::EndOfMedia)
            return;
        // if (playlist.count() > 1)
        ui->actionNext->trigger();
    });

    connect(&media_player, &QMediaPlayer::mediaStatusChanged, [&]() {
        if (media_player.mediaStatus() != QMediaPlayer::MediaStatus::LoadedMedia) {
            return;
        }

        player_bar.source_setted(true);
        int i = 0;

        ui->menuVideo_Track->clear();
        ui->menuAudio_Track->clear();
        ui->menuSubtitle_Track->clear();
        player_bar.setDuration(media_player.duration());

        for (auto track: media_player.videoTracks()) {
            auto resolution = track.value(QMediaMetaData::Resolution).toSize();
            auto codec = track.value(QMediaMetaData::VideoCodec).toString();

            QAction *action_track = new QAction(QString("%1x%2 %3")
                                                    .arg(resolution.width())
                                                    .arg(resolution.height())
                                                    .arg(codec));

            ui->menuVideo_Track->addAction(action_track);
            connect(action_track, &QAction::triggered, [&, i]() {
                media_player.setActiveVideoTrack(i);
            });
            i++;
        }
        i = 0;
        for (auto track: media_player.audioTracks()) {
            auto codec = track.value(QMediaMetaData::AudioCodec).toString();
            QAction *action_track = new QAction(QString(codec));

            ui->menuAudio_Track->addAction(action_track);
            connect(action_track, &QAction::triggered, [&, i]() {
                media_player.setActiveAudioTrack(i);
            });
            i++;
        }
        i = 0;
        for (auto track: media_player.subtitleTracks()) {
            auto title = track.value(QMediaMetaData::Title).toString();
            auto language = track.value(QMediaMetaData::Language).toString();
            QAction *action_track = new QAction(QString("%1 %2").arg(title).arg(language));

            ui->menuSubtitle_Track->addAction(action_track);
            connect(action_track, &QAction::triggered, [&, i]() {
                media_player.setActiveSubtitleTrack(i);
            });
            i++;
        }
    });

    connect(&playlist, &PlayList::selectedChanged, [&]() {
        player_bar.source_setted(false);
        auto cur_url = playlist.current();
        if (!cur_url.isEmpty() && cur_url.isValid() && cur_url == media_player.source())
            return;

        media_player.setSource(cur_url);
        //qDebug() << "source: " << media_player.source().toString().toStdString();
		playlist.add(QUrl("file:///tmp/[感知推荐] Love Story.mp3"));
		playlist.add(QUrl("file:///tmp/[感知推荐]I Guess I Just Feel Like.mp3"));
    });

    connect(ui->actionOpen_File, &QAction::triggered, [=]() {
        QFileDialog fdl;
        fdl.setFileMode(QFileDialog::ExistingFiles);
        fdl.setNameFilter("(*.mp3 *.wav *.mp4 *.avi *.mkv)");
        fdl.exec();
        auto files = fdl.selectedUrls();
        for (auto file: files) {
            // media_player.setSource("file://" + file);
            playlist.add(file);
        }
    });

    connect(ui->actionPlaylist, &QAction::triggered, [&]() {
        player_bar.toggle_playlist();
    });

    connect(ui->actionFullscreen, &QAction::triggered, [=]() {
        if (this->isFullScreen()) {
            this->showNormal();
            this->player_bar.show();
            ui->menubar->show();
        } else {
            this->showFullScreen();
            ui->menubar->hide();
            this->player_bar.hide();
            tool_bar.hide();
        }
    });


    connect(ui->actionPlay, &QAction::triggered, [&]() {
        // if (!media_player.isPlaying())
        if (media_player.playbackState() != QMediaPlayer::PlayingState)
            player_bar.toggle_play();
    });

    connect(ui->actionStop, &QAction::triggered, [&]() {
        // if (media_player.isPlaying())
        if (media_player.playbackState() != QMediaPlayer::PlayingState)
            player_bar.toggle_play();
    });

    connect(ui->actionPreference, &QAction::triggered, [&]() {
        // Setting *setting = new Setting();
        if (setting.isHidden())
            setting.show();
        else
            setting.hide();
    });

    connect(&player_bar, &PlayBar::volumeChanged, [&](int value) {
        media_player.audioOutput()->setVolume((double)(value) / 100.0);
    });

    connect(&media_player, &QMediaPlayer::positionChanged, &player_bar, &PlayBar::setPosition);
    connect(&media_player, &QMediaPlayer::durationChanged, &player_bar, &PlayBar::setDuration);
    connect(&player_bar, &PlayBar::zoomClicked, ui->actionFullscreen, &QAction::trigger);
    connect(&player_bar, &PlayBar::settingClicked, ui->actionPreference, &QAction::trigger);
    connect(&player_bar, &PlayBar::playlistChanged, [&](bool v) {
        if (v)
            tool_bar.show();
        else
            tool_bar.hide();
    });

    player_bar.toggle_playlist();
    connect(&player_bar, &PlayBar::playChanged, [&](bool v) {
        if (v)
            media_player.play();
        else
            media_player.pause();
    });
    connect(&player_bar, &PlayBar::stopClicked, ui->actionStop, &QAction::trigger);

    connect(&player_bar, &PlayBar::loopChanged, &playlist, &PlayList::set_loop);
    connect(&player_bar, &PlayBar::randomChanged, &playlist, &PlayList::set_shuffle);

    connect(&player_bar, &PlayBar::nextClicked, ui->actionNext, &QAction::trigger);
    connect(&player_bar, &PlayBar::prevClicked, ui->actionPrevious, &QAction::trigger);

    connect(&setting, &Setting::userChangedTheme,  this, &MainWindow::change_theme);
    connect(&setting, &Setting::userChangedTheme,  &player_bar, &PlayBar::change_theme);
    connect(&setting, &Setting::userChangedTheme,  &playlist, &PlayList::change_theme);
    connect(&setting, &Setting::userChangedTheme,  &modelchat, &ModelChat::change_theme);

    emit setting.userChangedTheme(0);

    connect(&modelchat, &ModelChat::video_cut_req, this, &MainWindow::ffmpeg_video_cut);
    connect(this, &MainWindow::ffmpeg_video_cut_ok, &modelchat, &ModelChat::chat_start);

    connect(ui->actionFeeling, &QAction::triggered, [&]() {
        if (modelchat.isHidden())
            modelchat.show();
        else
            modelchat.hide();
    });

    connect(&player_bar, &PlayBar::feelingClicked, ui->actionFeeling, &QAction::trigger);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    static std::atomic<bool> inited(false);
    static QTimer timer;
    timer.setSingleShot(true);
    if (!inited.exchange(true)) {
        connect(&timer, &QTimer::timeout, [&]() {
            if (!this->isFullScreen())
                return;
            if (player_bar.underMouse()) {
                timer.start(2500);
            } else {
                ui->menubar->hide();
                player_bar.hide();
            }
        });
    }
    ui->menubar->show();
    player_bar.show();
    if (this->isFullScreen()) {
        timer.start(2500);
    } else {
        timer.stop();
    }
    QWidget::mouseMoveEvent(event);
}
