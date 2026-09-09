#include "setting.h"
#include "ui_setting.h"

#include <QKeySequenceEdit>
#include	"shortcutkeyedititem.h"
#include	<QTimer>
#include	<QFile>

Setting::Setting(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Setting)
{
    ui->setupUi(this);

    // QTimer *qsstimer = new QTimer();
    // qsstimer->setInterval(1000);
    // qsstimer->setSingleShot(false);
    // connect(qsstimer, &QTimer::timeout, [=]() {
        // QString filepath("./qss/dark/%1.qss");
        // filepath = filepath.arg("setting");

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

    ui->shortcut->layout()->setAlignment(Qt::AlignTop);
    connect(this, &Setting::userChangedTheme,  this, &Setting::change_theme);
}

Setting::~Setting()
{
    delete ui;
}

void Setting::addShortcut(ShortcutKeyEditItem *shortcut) {
    connect(this, &Setting::userChangedTheme,  shortcut, &ShortcutKeyEditItem::change_theme);
    ui->shortcut->layout()->addWidget(shortcut);
}


bool Setting::event(QEvent *event) {
    if (event->type() == QEvent::ApplicationPaletteChange) {
        qDebug() << "theme changed" << " value=" << ui->themeSlider->value();
        emit userChangedTheme(ui->themeSlider->value());
        return true;
    }
    return QWidget::event(event);
}
