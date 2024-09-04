#ifndef WEATHER_H
#define WEATHER_H

#include <QWidget>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QPainter>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMovie>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QFile>
#include <QMap>
#include <QList>

#include "Day.h"
namespace Ui {
class weather;
}
class weather : public QWidget
{
    Q_OBJECT

public:
    explicit weather(QWidget *parent = nullptr);
    ~weather();
    Day day[7];
    QList<QLabel *> weekList;  //周几
    QList<QLabel *> dateList;  //日期
    QList<QLabel *> iconList;  //图标
    QList<QLabel *> weaList;  //天气
    QList<QLabel *> airqList; //空气质量
    QList<QLabel *> fxList;  //风向
    QList<QLabel *> flList;  //风力

    void updateUi();  //更新界面数据
private slots:
    void do_network(QNetworkReply *reply);
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

signals:
    void fanhui();  //返回信号
private:
    Ui::weather *ui;
    QMovie *movie;
    void paintEvent(QPaintEvent *event) override;
    QNetworkReply *reply;
    QString url;//网站
    QNetworkAccessManager *manager; //发送请求
    void jiexijsonNew(QByteArray data);  //解析得到的json

    QString getcityid(QString cityame);  //得到城市id
    QMap<QString,QString> cityid;  //用来查找城市id
    QString getWeaPix(QString wea);  //得到天气图片路径

    void drawHeight();  //画高温曲线
    void drawLow(); //低温
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;  //事件过滤器
};

#endif // WEATHER_H
