#include "mainwindow.h"
#include <QColorDialog>
#include <QErrorMessage>
#include <QFileDialog>
#include <QFontDialog>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QVBoxLayout>
#include "./ui_mainwindow.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    te_test = new QTextEdit;

    bt_filename = new QPushButton("獲取檔案名稱");
    QObject::connect(bt_filename, &QPushButton::clicked, [&]() {
        // 提取多個檔案名的對話框
        QStringList filenames = QFileDialog::getOpenFileNames(this, "選取檔案");
        for (int i = 0; i < filenames.length(); i++)
            te_test->append(filenames[i]);
    });

    bt_getcolor = new QPushButton("選取字體顏色");
    QObject::connect(bt_getcolor, &QPushButton::clicked, [&]() {
        QColor color = QColorDialog::getColor();
        te_test->setTextColor(color);
    });

    bt_getfont = new QPushButton("選取字體");
    QObject::connect(bt_getfont, &QPushButton::clicked, [&]() {
        bool flag;
        QFont font = QFontDialog::getFont(&flag);

        if (flag)
            te_test->setCurrentFont(font);
    });

    bt_getinput = new QPushButton("選取輸入");
    QObject::connect(bt_getinput, &QPushButton::clicked, [&]() {
        QString str = QInputDialog::getText(this, "輸入框", "輸入:");
        te_test->setText(str);
    });

    bt_error = new QPushButton("錯誤訊息");
    QObject::connect(bt_error, &QPushButton::clicked, [&]() {
        QErrorMessage err;
        err.setWindowTitle("錯誤");
        err.showMessage("錯誤: 說明錯誤原因1");
        err.showMessage("錯誤: 說明錯誤原因2");
        err.showMessage("錯誤: 說明錯誤原因3");
        err.exec();
    });

    bt_message = new QPushButton("訊息");
    QObject::connect(bt_message, &QPushButton::clicked, [&]() {
        QMessageBox::warning(this, "警告", "警告ㄉ原因", QMessageBox::Open, QMessageBox::Apply);
    });

    bt_progress = new QPushButton("進度條");
    QObject::connect(bt_progress, &QPushButton::clicked, [&]() {
        QProgressDialog prog("進度條", "取消", 0, 50000, this);
        prog.setWindowTitle("進度條視窗名稱");
        prog.show();
        for (int i{}; i < 50000; i++) {
            prog.setValue(i);
            QCoreApplication::processEvents();

            if (prog.wasCanceled())
                break;
        }
        //prog.exec();
    });

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(bt_filename);
    vbox->addWidget(bt_getcolor);
    vbox->addWidget(bt_getfont);
    vbox->addWidget(bt_getinput);
    vbox->addWidget(bt_error);
    vbox->addWidget(bt_message);
    vbox->addWidget(bt_progress);

    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addLayout(vbox);
    hbox->addWidget(te_test);
    setLayout(hbox);
}

Widget::~Widget()
{
}
