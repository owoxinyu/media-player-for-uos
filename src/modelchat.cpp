#include "modelchat.h"
#include "ui_modelchat.h"

#include	 <QLabel>
#include	 <QTimer>

ModelChat::ModelChat(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ModelChat)
{
    ui->setupUi(this);

    // connect(&fd, &QWebSocket::textMessageReceived, [&](const QString &msg) {
        // if (msg.contains("send_hash")) {
            // fd.sendTextMessage("{\"fn_index\":0,\"session_hash\":\"" + session_hash + "\"}");
        // }
        // if (msg.contains("send_data")) {
        // }
        // if (msg.contains("process_starts")) {
            // add_chatbox("处理开始...");
        // }
        // if (msg.contains("process_completed")) {
            // add_chatbox("处理完成...");
            // QJsonDocument doc = QJsonDocument::fromJson(msg.toUtf8());
            // QJsonArray arr = doc.object().value("output").toObject().value("data").toArray();
            // QString ret;
            // for(auto v: arr) {
                // if (v.isString())
                    // ret += v.toString();
            // }
            // add_chatbox(ret, 1);
        // }
        // qDebug() << "message=" << msg;
    // });
    // QTimer *qsstimer = new QTimer();
    // qsstimer->setInterval(1000);
    // qsstimer->setSingleShot(false);
    // connect(qsstimer, &QTimer::timeout, [=]() {
        // QString filepath("./qss/dark/%1.qss");
        // filepath = filepath.arg("modelchat");

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
    // });
    // qsstimer->start();


    processing_timer->setInterval(100);
    processing_timer->setSingleShot(false);

    connect(processing_timer, &QTimer::timeout, [=]() {
        this->processing_time += 100;
    });


}

ModelChat::~ModelChat()
{
    delete ui;
}

void ModelChat::add_chatbox(QString msg, int role) {
    switch(role) {
    case 0:
        msg = "系统: " + msg;
        break;
    case 1:
        msg = "模型: " + msg;
        break;
    case 2:
        msg = "用户: " + msg;
        break;
    }

    ui->listWidget->addItem(msg);
}


void ModelChat::on_connectButton_clicked() {
    fd.open(QUrl(ui->lineEdit->text()));
}

void ModelChat::chat_fn(const QString &msg) {
    if (msg.contains("send_hash")) {
        fd.sendTextMessage("{\"fn_index\":0,\"session_hash\":\"" + session_hash + "\"}");
    }
    if (msg.contains("send_data")) {
        QString n_msg = ui->textEdit->toPlainText();
        add_chatbox(n_msg, 2);
        n_msg.replace("\"", "");
        ui->textEdit->clear();
        QFile piece("/tmp/.video_piece.mp4");
        if (!piece.open(QFile::ReadOnly)) {
            add_chatbox("视频文件读取失败, 请重试...");
            return;
        }
        QString piece_data = QString::fromStdString(piece.readAll().toBase64().toStdString());

        fd.sendTextMessage("{\"data\":[\"" + piece_data + "\",\"" + n_msg + "\"],\"event_data\":null,\"fn_index\":0,\"session_hash\":\"" + session_hash +"\"}");
    }
    if (msg.contains("process_starts")) {
        processing_time = 0;
        processing_timer->start();
        add_chatbox("处理开始, 请耐心等待...");
    }
    if (msg.contains("process_completed")) {
        processing_timer->stop();
        add_chatbox(QString("处理完成, 本次处理使用了%1秒...").arg(processing_time / 1000, 0, 'f', 1));
        QJsonDocument doc = QJsonDocument::fromJson(msg.toUtf8());
        QJsonArray arr = doc.object().value("output").toObject().value("data").toArray();
        QString ret;
        for(auto v: arr) {
            if (v.isString())
                ret += v.toString();
        }
        add_chatbox(ret, 1);
        disconnect(&fd, &QWebSocket::textMessageReceived, this, &ModelChat::chat_fn);
        ui->sendButton->setDisabled(false);
    }
    qDebug() << "message=" << msg;
    //fd.sendTextMessage("{\"data\":[\"/root/test.mp4\",\"" + msg + "\"],\"event_data\":null,\"fn_index\":0,\"session_hash\":\"" + session_hash +"\"}");
}

void ModelChat::on_sendButton_clicked() {
    ui->sendButton->setDisabled(true);
    emit video_cut_req();
}

void ModelChat::chat_start() {
    fd.open(QUrl(ui->lineEdit->text()));
    connect(&fd, &QWebSocket::textMessageReceived, this, &ModelChat::chat_fn);
}
