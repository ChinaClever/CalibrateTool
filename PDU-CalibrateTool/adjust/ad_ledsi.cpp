/*
 *
 *  Created on: 2021年1月1日
 *      Author: Lzy
 */
#include "ad_ledsi.h"
#include "col_sithread.h"

Ad_LedSi::Ad_LedSi(QObject *parent) : QThread(parent)
{
    mPacket = sDataPacket::bulid();
    mModbus = Ad_Modbus::bulid(this);
    mItem = Ad_Config::bulid()->item;
}

Ad_LedSi *Ad_LedSi::bulid(QObject *parent)
{
    static Ad_LedSi* sington = nullptr;
    if(sington == nullptr)
        sington = new Ad_LedSi(parent);
    return sington;
}

bool Ad_LedSi::transmit(uchar *buf, int len)
{
    bool ret = false;
    uchar recv[64] = {0};
    len = mModbus->transmit(buf, len, recv, 20);
    if(len > 0) {
        if(recv[1] == buf[1]) ret = true;
    } else {
        qDebug() << "Ad_LedSi err " << len;
    }

    return ret;
}

bool Ad_LedSi::writeCmd(uchar fn, uchar line)
{
    uchar cmd[] = {0x7B, 0xA0, 0x00, 0x00, 0x00, 0xBB, 0xBB};
    int len = sizeof(cmd);

    cmd[1] = fn;
    cmd[2] = line;
    uchar crc = mModbus->getXorNumber(cmd, len-1);
    cmd[len-1] = crc;

    return transmit(cmd, len);
}


bool Ad_LedSi::writeDc()
{    
    bool ret = true;
    Col_SiThread *rtu = Col_SiThread::bulid();
    for(int i=0; i<3; ++i) {
        mPacket->status = tr("发送直流偏移命令！%1").arg(i+1);
        ret = writeCmd(0xA2, 0); ret = mModbus->delay(5);
        ret = rtu->readPduData(); if(!ret) ret = rtu->readPduData();
        if(mPacket->data->cur[0]) ret = false; else break; mModbus->delay(3);
    }

    if(!ret) return ret;
    mPacket->status = tr("设置标准源电流6A");
    ret = mSource->setCur(60, 10);
    if(ret) ret = writeCmd(0xA1, 0);

    return ret;
}

bool Ad_LedSi::writeAc()
{
    bool res = true;
    int line = mPacket->devType->lines;
    for(int i=0; i<line; ++i) {
        mPacket->status = tr("L%1 校准").arg(i+1);
        bool ret = mModbus->delay(3);
        if(ret) ret = writeCmd(0xA1, i);
        if(ret) {
            mPacket->status += tr("成功！");
        } else {
            mPacket->status += tr("失败！");
            res = false;
        }
    }

    return res;
}


bool Ad_LedSi::sentCmd()
{
    bool ret = true;
    sDevType *dt = mPacket->devType;
    if(DC == dt->ac) {
        ret = writeDc();
    } else {
        ret = writeAc();
    }

    return ret;
}

bool Ad_LedSi::startAdjust(YC_StandSource *source)
{
    mSource = source;
    mItem->step = Test_Ading;
    bool ret = sentCmd();
    if(ret) {
        mItem->step = Test_vert;
        mPacket->pass = Test_Success;
        mPacket->status = tr("校准返回正常！");
        ret = mModbus->delay(1);
    } else {
        mItem->step = Test_vert;
        mPacket->pass = Test_Fail;
    }

    return ret;
}
