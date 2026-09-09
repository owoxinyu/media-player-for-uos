#ifndef SETTING_H
#define SETTING_H

#include <QWidget>
#include <QApplication>
#include <QFile>
#include "shortcutkeyedititem.h"

namespace Ui {
class Setting;
}

class Setting : public QWidget
{
    Q_OBJECT

public slots:
    void change_theme(int idx) {
        QString filepath;
        switch(idx) {
        case 0:
            filepath = ":/theme/white/setting.qss";
            break;
        case 1:
            filepath = ":/theme/dark/setting.qss";
            break;
        default:
            QColor themeColor = QApplication::palette().highlight().color();
            if (themeColor.lightness() > 127)
                filepath = ":/theme/white/setting.qss";
            else
                filepath = ":/theme/dark/setting.qss";
        }

        QFile qssfile(filepath);
        if (qssfile.open(QFile::ReadOnly)) {
            QString s = QLatin1String(qssfile.readAll());
            setStyleSheet(s);
        }
    }
    void on_themeSlider_valueChanged(int value) {
        emit userChangedTheme(value);
    }

signals:
    void userChangedTheme(int);

public:
    explicit Setting(QWidget *parent = nullptr);
    ~Setting();

    void addShortcut(ShortcutKeyEditItem *);
    bool event(QEvent *event) override;

private:
    Ui::Setting *ui;
};

#endif // SETTING_H
