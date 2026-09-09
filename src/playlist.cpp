#include "playlist.h"
#include "ui_playlist.h"

#include	<QDebug>
#include <QListWidgetItem>
#include <QRandomGenerator>
#include	<QTimer>
#include	<QFile>


PlayList::PlayList(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayList)
{
    ui->setupUi(this);

    // QTimer *qsstimer = new QTimer();
    // qsstimer->setInterval(1000);
    // qsstimer->setSingleShot(false);
    // connect(qsstimer, &QTimer::timeout, [=]() {
        // QString filepath("./qss/dark/%1.qss");
        // filepath = filepath.arg("playlist");

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


    connect(ui->listWidget, &QListWidget::currentRowChanged, [&](int row) {
        if (row == -1)
            return;
        emit selectedChanged(ui->listWidget->item(row)->data(Qt::UserRole).toUrl());
    });
}

PlayList::~PlayList()
{
    delete ui;
}

// void PlayList::shuffle() {
    // for (auto url: urls) {
        // QListWidgetItem *item = new QListWidgetItem();
        // item->setText(url.fileName());
        // item->setData(Qt::UserRole, url.toString());

        // ui->listWidget->addItem(item);
        // if (!_last_item_text.isNull() && !_last_item_text.isEmpty() && _last_item_text == url.toString())
            // ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);
    // }
// }

void PlayList::next() {
    if (ui->listWidget->count() == 0)
        return;

    int _row = ui->listWidget->currentRow();

    if (!loop) {
        if (shuffle)
            _row = QRandomGenerator::system()->generate() % ui->listWidget->count();
        else if (_row == -1 ||_row == ui->listWidget->count() - 1)
            _row = 0;
        else
            _row++;
    }

    ui->listWidget->setCurrentRow(_row);
}

void PlayList::prev() {
    int _row = ui->listWidget->currentRow();
    if (_row == -1 || ui->listWidget->count() == 0)
        return;
    if (_row == 0)
        _row = ui->listWidget->count() - 1;
    else
        _row--;
    ui->listWidget->setCurrentRow(_row);
}

QUrl PlayList::current() {
    if (ui->listWidget->currentRow() == -1)
        return QUrl();
    return ui->listWidget->item(ui->listWidget->currentRow())->data(Qt::UserRole).toUrl();
}

void PlayList::add(QUrl url) {
    urls.append(url);


    QListWidgetItem *item = new QListWidgetItem();
    item->setText(url.fileName());
    item->setData(Qt::UserRole, url.toString());

    if (shuffle) {
        ui->listWidget->insertItem(ui->listWidget->currentRow() + 1, item);
    } else {
        ui->listWidget->addItem(item);
    }
}
