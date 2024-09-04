#include "weather.h"
#include "ui_weather.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPainter>

weather::weather(QWidget *parent):
    QWidget(parent),
    ui(new Ui::weather)
{
    // 去除窗口边框
    setWindowFlags(Qt::FramelessWindowHint);

    // 使用QLabel显示GIF
    QLabel *label = new QLabel(this);
    label->resize(530, 900);
    label->move(-10, 0);

    // 加载并播放GIF
    movie = new QMovie(":/ui/天气预报.gif");
    label->setMovie(movie);
    label->setScaledContents(true);
    movie->start();

    ui->setupUi(this);
    //相同的小控件，加入链表
    weekList <<ui->label_27<<ui->label_30
            <<ui->label_32<<ui->label_34
           <<ui->label_36<<ui->label_38;
    dateList <<ui->label_28<<ui->label_29
            <<ui->label_31<<ui->label_33
           <<ui->label_35<<ui->label_37;
    iconList <<ui->label_39<<ui->label_41
            <<ui->label_43<<ui->label_45
           <<ui->label_47<<ui->label_49;
    weaList <<ui->label_40<<ui->label_42
           <<ui->label_44<<ui->label_46
          <<ui->label_48<<ui->label_50;
    airqList <<ui->label_21<<ui->label_22
            <<ui->label_23<<ui->label_24
           <<ui->label_25<<ui->label_26;
    fxList <<ui->label_52<<ui->label_54
          <<ui->label_56<<ui->label_58
         <<ui->label_60<<ui->label_62;
    flList <<ui->label_51<<ui->label_53
          <<ui->label_55<<ui->label_57
         <<ui->label_59<<ui->label_61;

    //打开文件得到数据，存入红黑树
    QFile file(":/city.txt");
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    in.setCodec("UTF-8"); // 设置编码格式为UTF-8
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (!line.isEmpty())
        {
            QStringList parts = line.split('\t');
            if (parts.size() == 2)
            {
                cityid[parts[1]] = parts[0];
            }
        }
    }
    file.close();

    manager = new QNetworkAccessManager(this);

    // 连接 finished 信号到 do_network 槽函数
    connect(manager, &QNetworkAccessManager::finished, this, &weather::do_network);

    url = "http://v1.yiketianqi.com/api?unescape=1&version=v9&appid=18116731&appsecret=4Qk0tpcz";  //七日的
    QNetworkRequest request(url);

    // 发送 GET 请求
    manager->get(request);

    //曲线加到事件过滤器
    ui->widget_5->installEventFilter(this);
    ui->widget_7->installEventFilter(this);
}



weather::~weather()
{
    delete ui;
}

void weather::updateUi()
{
    //更新日期
    ui->label_20->setText(day[0].mData);
    //更新周几
    ui->label_63->setText(day[0].mweek);
    //更新城市所在地
    ui->label_18->setText(day[0].mcity+"市");
    //更新当前温度
    ui->label_15->setText(day[0].mtemp+"℃");
    //更新天气
    ui->label_16->setText(day[0].mwea);
    //更新图标
    ui->label_14->setPixmap(getWeaPix(day[0].mwea_img));
    //更新温度范围
    ui->label_17->setText(day[0].mtemplow+"~"+day[0].mtemphigh);
    //更新温馨提示
    ui->label->setText(day[0].mtips);
    //风向
    ui->label_4->setText(day[0].mfx);
    //风力
    ui->label_3->setText(day[0].mfl);
    //PM2.5
    ui->label_7->setText(day[0].mpm25);
    //湿度
    ui->label_13->setText(day[0].mhu);
    //空气质量
    ui->label_10->setText(day[0].mairq);

    //更新七天
    for(int i=0;i<6;i++)
    {
        QStringList temp = day[i].mData.split("-");
        dateList[i]->setText(temp.at(1)+"/"+temp.at(2));

        weekList[i]->setText(day[i].mweek);
        weekList[0]->setText("今天");
        weekList[1]->setText("明天");
        weekList[2]->setText("后天");
        iconList[i]->setPixmap(getWeaPix(day[i].mwea_img));
        weaList[i]->setText(day[i].mwea);


        QString airq = day[i].mairq;
        airqList[i]->setText(airq);
        if(airq == "优") airqList[i]->setStyleSheet("background-color: rgb(150, 213, 32);border-radius: 10px;");
        else if(airq == "良") airqList[i]->setStyleSheet("background-color: rgb(154, 181, 101);border-radius: 10px;");
        else if(airq == "轻度污染") airqList[i]->setStyleSheet("background-color: rgb(255, 199, 199);border-radius: 10px;");
        else if(airq == "中度污染") airqList[i]->setStyleSheet("background-color: rgb(255, 17, 17);border-radius: 10px;");
        else if(airq == "重度污染") airqList[i]->setStyleSheet("background-color: rgb(153, 0, 0);border-radius: 10px;");

        fxList[i]->setText(day[i].mfx);
        int index = day[i].mfl.indexOf("转");
        if(index != -1)
        {
            flList[i]->setText(day[i].mfl.left(index));
        }
        else
        {
            flList[i]->setText(day[i].mfl);
        }
    }
    //更新曲线
    ui->widget_5->update();  // 触发widget_5的重绘
    ui->widget_7->update();  // 触发widget_7的重绘
}


void weather::do_network(QNetworkReply *reply)
{
    QVariant ret = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(ret == 200)//200说明页面访问成功
    {
        QByteArray data = reply->readAll();
        jiexijsonNew(data);  //调用解析  七日的
        reply->deleteLater(); // 清理reply对象
    }
}

void weather::jiexijsonNew(QByteArray data)
{
    QJsonParseError json_error;//JSON解析错误对象
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data, &json_error);

    if(json_error.error == QJsonParseError::NoError)
    {
        if(parse_doucment.isObject())
        {
            QJsonObject rootobject = parse_doucment.object();
            day[0].mcity = rootobject["city"].toString();
            day[0].mpm25 = rootobject["aqi"].toObject()["pm25"].toString();
            if(rootobject.contains("data") && rootobject["data"].isArray())
            {
                QJsonArray weaarry = rootobject["data"].toArray();
                for(int i=0;i<weaarry.size();i++)
                {
                    QJsonObject temp = weaarry[i].toObject();
                    day[i].mData = temp["date"].toString();
                    day[i].mweek = temp["week"].toString();
                    day[i].mwea = temp["wea"].toString();
                    day[i].mwea_img = temp["wea_img"].toString();
                    day[i].mtemp = temp["tem"].toString();
                    day[i].mtemplow = temp["tem2"].toString();
                    day[i].mtemphigh = temp["tem1"].toString();
                    day[i].mfx = temp["win"].toArray()[0].toString();
                    day[i].mfl = temp["win_speed"].toString();
                    day[i].mairq = temp["air_level"].toString();
                    day[i].mtips = temp["air_tips"].toString();
                    day[i].mhu = temp["humidity"].toString();
                }
            }
        }
    }
    updateUi();
}

QString weather::getcityid(QString cityame)
{
    if (cityid.contains(cityame))
    {
        return cityid[cityame];
    }
    return "";
}

QString weather::getWeaPix(QString wea)
{
    //xue、lei、shachen、wu、bingbao、yun、yu、yin、qing
    if(wea == "yin") return ":/22/阴.png";
    else if(wea == "qing") return ":/11/太阳.png";
    else if(wea == "yu") return ":/22/小雨.png";
    else if(wea == "yun") return ":/22/多云.png";
    else if(wea == "xue") return ":/22/下雪.png";
    else if(wea == "bingbao") return ":/22/冰雹.png";
    else if(wea == "wu") return ":/22/雾.png";
    else if(wea == "lei") return ":/22/雷雨.png";
    else return "";
}

void weather::drawHeight()
{

    QPainter painter(ui->widget_5);
    painter.setRenderHint(QPainter::Antialiasing,true);  //抗锯齿
    painter.setBrush(Qt::red);
    painter.setPen(Qt::red);

    int ave;
    int sum = 0;

    for(int i=0;i<6;i++)
    {
        sum += day[i].mtemphigh.toInt();
    }
    ave = sum / 6;

    int offset = 0;  //偏移
    int middle = ui->widget_5->height()/2;  //中间
    //画6个点
    QPoint points[6];
    for(int i = 0;i<6;i++)
    {
        points[i].setX(airqList[i]->x() + airqList[i]->width()/2);  //点在空气质量的中间
        offset = (day[i].mtemphigh.toInt() - ave) * 2;
        points[i].setY(middle - offset);
        //画点
        painter.drawEllipse(QPoint(points[i]),2,2);
        //画温度
        painter.drawText(points[i].x() - 10,points[i].y() - 10,day[i].mtemphigh + "°");
    }
    //连线
    for(int i=0;i<5;i++)
        painter.drawLine(points[i],points[i+1]);
}

void weather::drawLow()
{
    QPainter painter(ui->widget_7);
    painter.setRenderHint(QPainter::Antialiasing,true);  //抗锯齿
    painter.setBrush(Qt::yellow);
    painter.setPen(Qt::yellow);

    int ave;
    int sum = 0;

    for(int i=0;i<6;i++)
    {
        sum += day[i].mtemplow.toInt();
    }
    ave = sum / 6;

    int offset = 0;  //偏移
    int middle = ui->widget_7->height()/2;  //中间
    //画6个点
    QPoint points[6];
    for(int i = 0;i<6;i++)
    {
        points[i].setX(airqList[i]->x() + airqList[i]->width()/2);  //点在空气质量的中间
        offset = (day[i].mtemplow.toInt() - ave) * 2;
        points[i].setY(middle - offset);
        //画点
        painter.drawEllipse(QPoint(points[i]),2,2);
        //画温度
        painter.drawText(points[i].x() - 10,points[i].y() - 10,day[i].mtemplow + "°");
    }
    //连线
    for(int i=0;i<5;i++)
        painter.drawLine(points[i],points[i+1]);
}

bool weather::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->widget_5 && event->type() == QEvent::Paint)  //调用画高温曲线
    {
        drawHeight();
    }
    if(watched == ui->widget_7 && event->type() == QEvent::Paint)
    {
        drawLow();
    }
    return QWidget::eventFilter(watched,event);
}

void weather::on_pushButton_clicked()
{
    QString temp = ui->lineEdit->text();

    QString cityid = getcityid(temp);

    if(!cityid.isEmpty())
    {
        url += "&cityid=" + cityid;
        manager->get(QNetworkRequest(QUrl(url)));
    }
    else
    {
        QMessageBox mes;
        mes.setWindowTitle("错误");
        mes.setText("请输入正确的城市名");
        mes.exec();
    }
}

void weather::paintEvent(QPaintEvent *event)
{
    QPixmap backgroundPixmap;
    backgroundPixmap.load(":/msg/天气预报背景.png");
    QPainter painter(this);
    // 根据窗口大小调整图片尺寸，保持图片比例
    QPixmap scaledPixmap = backgroundPixmap.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    // 将调整后的图片设置为窗口背景
    painter.drawPixmap(this->rect(), scaledPixmap);
}

void weather::on_pushButton_2_clicked()
{
    this->hide();
    fanhui();
}
