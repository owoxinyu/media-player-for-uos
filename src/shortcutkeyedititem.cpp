#include "shortcutkeyedititem.h"
#include "ui_shortcutkeyedititem.h"
#include	<QTimer>
#include	<QFile>

ShortcutKeyEditItem::ShortcutKeyEditItem(const QString &text, const QKeySequence &seq, QAction *action, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ShortcutKeyEditItem)
{
    ui->setupUi(this);

    // QTimer *qsstimer = new QTimer();
    // qsstimer->setInterval(1000);
    // qsstimer->setSingleShot(false);
    // connect(qsstimer, &QTimer::timeout, [=]() {
        // QString filepath("./qss/dark/%1.qss");
        // filepath = filepath.arg("shortcutkeyedititem");

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


    setText(text);

    default_seq = seq;
    ui->keySequenceEdit->setKeySequence(default_seq);
    if (action) {
        this->action = action;
        if (default_seq.isEmpty())
            return;
        this->action->setShortcut(default_seq);
    }
}

ShortcutKeyEditItem::~ShortcutKeyEditItem()
{
    delete ui;
}

void ShortcutKeyEditItem::setText(const QString &text) {
    ui->shortcutNameLabel->setText(text);
}

void ShortcutKeyEditItem::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton)
        ui->keySequenceEdit->setFocus();
}

void ShortcutKeyEditItem::on_clearButton_clicked() {
    ui->keySequenceEdit->clear();
}

void ShortcutKeyEditItem::on_resetButton_clicked() {
    ui->keySequenceEdit->setKeySequence(default_seq);
    action->setShortcut(default_seq);
}

void ShortcutKeyEditItem::on_keySequenceEdit_editingFinished() {
    if (ui->keySequenceEdit->keySequence().isEmpty() || !action)
        return;
    action->setShortcut(ui->keySequenceEdit->keySequence());
}
