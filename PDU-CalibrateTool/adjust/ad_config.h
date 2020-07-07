#ifndef ADJUSTCONFIG_H
#define ADJUSTCONFIG_H

#include "configbase.h"
#include "serialstatuswid.h"

enum {
    Test_Function, // 功能测试
    Test_Start, // 开始
    Test_vert, // 验证
    Test_End, // 完成
    Test_Over, // 终止

    Collect_Start,

    Test_Success=1,
    Test_Fail,
};



struct sConfigItem
{
    sConfigItem() {step=0; vol=220; currentNum=0;}
    SerialPort *serial; // 串口对象
    SerialPort *source; // 标准源

    uchar step; // 步骤
    uchar addr;

    ushort vol;
    ushort volErr, curErr; // 电流误差
    int logCount;

    uchar pcNum;
    ushort currentNum; // 当天序号
    QString currentName; // 当前用户名称
};


class Ad_Config : public ConfigBase
{
    Ad_Config();
public:
    static Ad_Config *bulid();
    sConfigItem *item;

    void setCurrentNum();
    QString initName();
    void setName(QString str);
protected:
    bool getDate();
    void setDate();

    void initCurrentNum();
};

#endif // ADJUSTCONFIG_H
