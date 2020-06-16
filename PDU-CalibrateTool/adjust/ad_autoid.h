#ifndef AD_AUTOID_H
#define AD_AUTOID_H

#include "ad_modbus.h"

class Ad_AutoID : public QThread
{
    Q_OBJECT
    explicit Ad_AutoID(QObject *parent = nullptr);
public:
     static Ad_AutoID *bulid(QObject *parent = nullptr);

    bool readDevType();

protected:
    void initReadType(sRtuItem &it);
    bool analysDevType(uchar *buf, int len);

private:
    sConfigItem *mItem;
    Ad_Modbus *mModbus;
    sDataPacket *mPacket;
};

#endif // AD_AUTOID_H