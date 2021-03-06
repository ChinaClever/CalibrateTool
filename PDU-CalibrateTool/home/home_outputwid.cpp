/*
 *
 *  Created on: 2020年10月1日
 *      Author: Lzy
 */
#include "home_outputwid.h"
#include "ui_home_outputwid.h"
#include "yc_dc107.h"

Home_OutputWid::Home_OutputWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Home_OutputWid)
{
    ui->setupUi(this);
    this->setEnabled(false);
    groupBox_background_icon(this);
    mData = sDataPacket::bulid()->data;
    QTimer::singleShot(100,this,SLOT(initFunSlot()));
}

Home_OutputWid::~Home_OutputWid()
{
    delete ui;
}

void Home_OutputWid::initFunSlot()
{
    this->setEnabled(true);
    on_devBox_currentIndexChanged(1);
}

void Home_OutputWid::on_onAllBtn_clicked()
{
    if(mThread)mThread->openAllSwitch();
}

void Home_OutputWid::on_eleAllBtn_clicked()
{
    if(mThread)mThread->factorySet();
    //mThread->clearAllEle();
     InfoMsgBox box(this, tr("延时已清除"));
}

void Home_OutputWid::on_closeAllBtn_clicked()
{
    if(mThread)mThread->closeAllSwitch();
}

void Home_OutputWid::on_openBtn_clicked()
{
    if(mThread) {
        int id = ui->spinBox->value();
        mThread->openOnlySwitch(id-1);
    }
}

void Home_OutputWid::on_closeBtn_clicked()
{
    if(mThread) {
        int id = ui->spinBox->value();
        mThread->closeOutputSwitch(id-1);
    }
}

void Home_OutputWid::on_eleBtn_clicked()
{
    if(mThread) {
        int id = ui->spinBox->value();
        mThread->setClearEle(id-1);
    }
}

void Home_OutputWid::workDownSlot()
{
    int size = ui->opSpin->value();
    if(mId < size) {
        mThread->openOnlySwSlot(mId++);
        QTimer::singleShot(1000,this,SLOT(workDownSlot()));
    } else {
        on_onAllBtn_clicked();
    }
}

void Home_OutputWid::on_swAllBtn_clicked()
{
    if(!mThread) return;
    QuMsgBox box(this, tr("确认是否开启自动切换功能?"));
    if(box.Exec()) {
        QTimer::singleShot(100,this,SLOT(workDownSlot())); mId=0;
    }
}


void Home_OutputWid::on_onBtn_clicked()
{
    QuMsgBox box(this, tr("是否需要校准上电?"));
    if(!box.Exec()) return;
    int ret = YC_Ac92b::bulid(this)->powerOn();
    if(ret <= 0) {
        CriticalMsgBox box(this, tr("标准源上电失败!"));
    }
}

void Home_OutputWid::on_downBtn_clicked()
{
    YC_Ac92b::bulid(this)->powerDown();
    InfoMsgBox box(this, tr("标榜源已下电"));
}

void Home_OutputWid::on_devBox_currentIndexChanged(int index)
{
    if(index) {
        mThread = Ctrl_ZpduThread::bulid(this);
    } else {
        mThread = Ctrl_MpduThread::bulid(this);
    }
}
