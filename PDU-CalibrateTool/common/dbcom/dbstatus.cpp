#include "dbstatus.h"

DbStatus::DbStatus()
{
    createTable();
    tableTile = tr("校准过程日志");
    //hiddens <<  9;
    //headList << tr("设备类型") << tr("操作员") << tr("序列码") << tr("校准状态") ;
}

DbStatus *DbStatus::bulid()
{
    static DbStatus* sington = nullptr;
    if(sington == nullptr)
        sington = new DbStatus();
    return sington;
}
