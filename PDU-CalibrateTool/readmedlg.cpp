﻿/*
 *
 *  Created on: 2020年10月1日
 *      Author: Lzy
 */
#include "readmedlg.h"
#include "ui_readmedlg.h"
#include <QtCore>
#include "common.h"

ReadMeDLg::ReadMeDLg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReadMeDLg)
{
    ui->setupUi(this);
    initText();
    groupBox_background_icon(this);
}

ReadMeDLg::~ReadMeDLg()
{
    delete ui;
}

void ReadMeDLg::initText()
{
    QFile file("readme.txt");
    bool ret = file.exists();
    ret = file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(ret) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            ui->textEdit->append(line);
        }
        file.close();
    } else {
        qDebug() << "open readme.txt err";
    }
}

void ReadMeDLg::on_okBtn_clicked()
{
    this->close();
}
