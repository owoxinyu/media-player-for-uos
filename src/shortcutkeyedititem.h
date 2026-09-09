#ifndef SHORTCUTKEYEDITITEM_H
#define SHORTCUTKEYEDITITEM_H

#include <QWidget>
#include <QAction>
#include <QApplication>
#include <QKeySequence>
#include <QShortcut>
#include <QFile>
#include <QMouseEvent>

namespace Ui {
class ShortcutKeyEditItem;
}

class ShortcutKeyEditItem : public QWidget
{
    Q_OBJECT

public:
    explicit ShortcutKeyEditItem(const QString &, const QKeySequence &, QAction *action, QWidget *parent = nullptr);
    ~ShortcutKeyEditItem();

    void setText(const QString &);
    void setAction(QAction *action);
    void setEnabled(bool);

protected:
    void mousePressEvent(QMouseEvent *) override;

public slots:
    void on_clearButton_clicked();
    void on_resetButton_clicked();
    void on_keySequenceEdit_editingFinished();
        void change_theme(int idx) {
        QString filepath;
        switch(idx) {
        case 0:
            filepath = ":/theme/white/shortcutkeyedititem.qss";
            break;
        case 1:
            filepath = ":/theme/dark/shortcutkeyedititem.qss";
            break;
        default:
            QColor themeColor = QApplication::palette().highlight().color();
            if (themeColor.lightness() > 127)
                filepath = ":/theme/white/shortcutkeyedititem.qss";
            else
                filepath = ":/theme/dark/shortcutkeyedititem.qss";
        }

        QFile qssfile(filepath);
        if (qssfile.open(QFile::ReadOnly)) {
            QString s = QLatin1String(qssfile.readAll());
            setStyleSheet(s);
        }
    }

signals:
    void shortcutChanged(QKeySequence seq);
    void shortcutActivated();

private:
    Ui::ShortcutKeyEditItem *ui;
    QKeySequence default_seq;
    QAction *action = nullptr;
};

#endif // SHORTCUTKEYEDITITEM_H
