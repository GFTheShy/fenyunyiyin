#ifndef DAY_H
#define DAY_H
#include <QString>
class Day
{
public:
    Day() {};
    QString mData;  //日期 1
    QString mweek;  //周几 1
    QString mcity;  //城市
    QString mtemp;  //当前温度 1
    QString mwea;  //天气 1
    QString mwea_img; //天气图标类型 1
    QString mtemplow;  //最低温 1
    QString mtemphigh;  //最高温 1

    QString mtips;  //小建议 1
    QString mfx;  //风向 1
    QString mfl;  //风力 1
    QString mpm25;  //PM2.5
    QString mhu;  //湿度
    QString mairq;  //空气质量 1
};
#endif // DAY_H
