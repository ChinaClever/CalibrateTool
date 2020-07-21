/*
 *
 *  Created on: 2020年10月1日
 *      Author: Lzy
 */
#include "ad_corethread.h"
#include "dblogs.h"
extern QString user_land_name();

Ad_CoreThread::Ad_CoreThread(QObject *parent) : QThread(parent)
{
    isRun = false;
    mPacket =sDataPacket::bulid();
    mItem = Ad_Config::bulid()->item;

    mModbus = Ad_Modbus::bulid(this);
    mAutoID = Ad_AutoID::bulid(this);
    mAdjust = Ad_Adjusting::bulid(this);
    mResult = Ad_Resulting::bulid(this);
    mSource = YC_StandSource::bulid(this);
    mSn = SN_ManageThread::bulid(this);
}

Ad_CoreThread *Ad_CoreThread::bulid(QObject *parent)
{
    static Ad_CoreThread* sington = nullptr;
    if(sington == nullptr)
        sington = new Ad_CoreThread(parent);
    return sington;
}

void Ad_CoreThread::startAdjust()
{
    if(!isRun) {
        mItem->step = Test_Start;
        start();
    }
}

void Ad_CoreThread::startCollect()
{
    if(!isRun) {
        mItem->step = Collect_Start;
        start();
    }
}

void Ad_CoreThread::startResult()
{
    if(!isRun) {
        mItem->step = Test_vert;
        start();
    }
}

void Ad_CoreThread::collectData()
{
    mPacket->status = tr("数据采集");
    bool ret = readDevInfo();
    if(!ret) return;

    Col_CoreThread *th = mResult->initThread();
    while(mItem->step != Test_Over) {
        th->readPduData();
        mResult->resTgData();
        delay(2);
    }
}

void Ad_CoreThread::verifyResult()
{
    mPacket->status = tr("采集自动验证");
    bool ret = readDevInfo();
    if(ret) {
        Db_Tran db; // 事务操作
        mResult->resEnter();
    }
}

void Ad_CoreThread::writeLog()
{
    sLogItem it;

    it.dev =mPacket->dev_type.split("_").first();
    it.user = user_land_name();
    it.sn = mPacket->sn;
    if(mPacket->pass == Test_Success) {
        it.result = tr("通过");
    } else {
        it.result = tr("失败：%1").arg(mPacket->status);
    }
    DbLogs::bulid()->insertItem(it);
}

bool Ad_CoreThread::readDevInfo()
{
    //    mSource->setVol();
    //    bool ret = mAutoID->readDevType();//读取设备类型
    //    if(ret) {
    //        ret = mSn->snEnter();//写入序列号
    //    }

    //    return ret;


    //////////////===================
    sDevType *mDt = mPacket->devType;
    mDt->devType = MPDU;
    mDt->ac = AC;
    mDt->specs = Mn;
    mDt->lines = 1;

    return true;

}

void Ad_CoreThread::workDown()
{
    Db_Tran db; // 事务操作
    mPacket->status = tr("已启动校准！");
    bool ret = readDevInfo(); if(!ret) return;
    mModbus->writeLog(ret);  // 序列号操作成功，才能记录日志

    mPacket->status = tr("复位单片机");
    mSource->powerReset();//控制标准源下电到上电
    ret = mAdjust->startAdjust();
    if(ret) {
        mPacket->status = tr("开始自动验证");
        ret = mResult->resEnter();
    }

    writeLog();   //记录校准设备校准成功还是校准失败
    mItem->step = Test_End;
}

void Ad_CoreThread::run()
{
    if(!isRun) {
        isRun = true;
        mPacket->clear();
        mPacket->pass = 0;

        switch (mItem->step) {
        case Test_Start: workDown(); break;
        case Collect_Start: collectData(); break;
        case Test_vert: verifyResult(); break;
        }

        mSource->powerDown();
        isRun = false;
    } else {
        qDebug() << "AdjustCoreThread run err" << isRun;
    }
}
