/*
 *
 *  Created on: 2020年10月1日
 *      Author: Lzy
 */
#include "ad_autoid.h"

Ad_AutoID::Ad_AutoID(QObject *parent) : QThread(parent)
{        
    mPacket = sDataPacket::bulid();
    mModbus = Ad_Modbus::bulid(this);
    mItem = Ad_Config::bulid()->item;
    mDevType = Ad_DevType::bulid();
    mDt = mPacket->devType;
}

Ad_AutoID *Ad_AutoID::bulid(QObject *parent)
{
    static Ad_AutoID* sington = nullptr;
    if(sington == nullptr)
        sington = new Ad_AutoID(parent);
    return sington;
}

void Ad_AutoID::initReadType(sRtuItem &it)
{
    it.addr = mItem->addr;
    it.fn = 0x03;
    it.reg = 0xA001;
    it.num = 2;
}

bool Ad_AutoID::analysDevType(uchar *buf, int len)
{
    if(len != 4) {
        mPacket->status = tr("通讯错误，未能正确识别执行板/表头 %1").arg(len);
        return false;
    }

    uint id = 0;
    for(int i=0; i<len; ++i) {
        id *= 256;  id += buf[i];
    }

    bool ret = mDevType->analysDevType(id);
    if(!ret){
        mPacket->status = QObject::tr("不支持此设备类型 ID是%1").arg(id);
    }

    return ret;
}

bool Ad_AutoID::readDevId()
{
    sRtuItem it;
    initReadType(it);

    uchar recv[8] = {0};
    int len = mModbus->rtuRead(&it, recv);
    if(!len){
        mPacket->status = tr("再次读取设备ID");
        mModbus->delay(6);
        len = mModbus->rtuRead(&it, recv);
    }

    if(0 == len){
        mPacket->status = tr("修改波特率，读取设备ID");
        bool ret = mModbus->changeBaudRate(); // 自动转变波特率
        if(!ret) {mModbus->delay(2); len = mModbus->rtuRead(&it, recv);}
        if(!len) {
            mModbus->changeBaudRate(); mModbus->delay(2);
            len = mModbus->rtuRead(&it, recv);
        }
    }

    return analysDevType(recv, len);
}

bool Ad_AutoID::readDevType()
{
    mPacket->status = tr("正在识别模块类型！");
    bool ret = readDevId();
    if(ret) {
        mPacket->status = tr("识别模块成功！");
        ret = mModbus->delay(1);

        if(IP_PDU == mDt->devType){
            mPacket->status = tr("读取IP模块代号！");
            ret = mModbus->delay(8);
            ret = readDevId();
        }
    }else{
        mItem->step = Test_End;
        mPacket->pass = Test_Fail;
    }

    return ret;
}

