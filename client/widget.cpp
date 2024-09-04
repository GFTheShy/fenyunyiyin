#include "widget.h"
#include "ui_widget.h"

#include <QContextMenuEvent>
#include <QMenu>
#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{

    // 去除窗口边框
    setWindowFlags(Qt::FramelessWindowHint);

    // 使用QLabel显示GIF
    QLabel *label = new QLabel(this);
    label->resize(624, 467);
    label->move(0, 0); // 根据需要调整位置

    // 加载并播放GIF
    movie = new QMovie(":/ui/山水.gif");
    label->setMovie(movie);
    label->setScaledContents(true);
    movie->start();

    // 设置窗口为椭圆形
    setMask(createEllipticalMask());

    ui->setupUi(this);

    pzhuce = new Form();
    connect(pzhuce,SIGNAL(fanhui()),this,SLOT(dofanhui()));

    connect(pzhuce,SIGNAL(user_zhuce(std::string,std::string,bool)),this,SLOT(zhuce(std::string,std::string,bool)));

    pmenu = new choose();
    connect(pmenu,SIGNAL(fanhui()),this,SLOT(dofanhui()));
}

Widget::~Widget()
{
    delete ui;
    delete movie;
}

QBitmap Widget::createEllipticalMask() const
{
    // 使用固定的窗口大小创建椭圆形遮罩
    QBitmap mask(624, 467);
    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(mask.rect(), Qt::color0);
    painter.setBrush(Qt::color1);
    painter.drawEllipse(0, 0, 624, 467);
    return mask;
}

void Widget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = new QMenu(this);
    menu->addAction("关闭窗口", this, &Widget::closeQuestion);
    menu->exec(event->globalPos());
}

void Widget::closeQuestion()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "关闭窗口", "您确定要关闭窗口吗？",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        close();
    }
}

void Widget::doconnected()
{
    QMSG msg;
    msg.type = 2;
    QString str = ui->username->text();//名字
    std::string str1 = str.toStdString();

    QString str_pw = ui->passwd->text();//密码
    std::string str_pw1 = str_pw.toStdString();//转成标准的字符串才能获得里面的char *


    //定义一个注册结构体
    user rn(str1.c_str(),str_pw1.c_str(),0);
    memcpy(msg.buf,&rn,sizeof(rn));//把注册结构体装入传输结构体

    client->write((char*)&msg,sizeof(msg));
}


void Widget::on_pushButton_clicked()
{
    client = new QTcpSocket(this);
    //教室网络
    client->connectToHost("192.168.50.159",9000);
    //宿舍网络
//    client->connectToHost("192.168.0.6",9000);
    connect(client,&QTcpSocket::connected,this,&Widget::doconnected);
    //readyRead是有消息可读的信号
    connect(client, &QTcpSocket::readyRead, this, &Widget::onReadyRead);
}

void Widget::onReadyRead()
{
    char buf[128];
    client->read(buf,128);

    if(strcmp(buf,"get")==0)  //登录成功 跳到选择界面
    {
        this->hide();
        pmenu->show();
    }
    else  //失败，提示
    {
        QMessageBox messageBox;
        messageBox.setIconPixmap(QPixmap(":/msg/错误.png"));
        messageBox.setText("<font color='red'>账号或密码错误！</font>");
        messageBox.setWindowTitle("错误");
        messageBox.setStandardButtons(QMessageBox::Ok);
        messageBox.exec();
    }
}

void Widget::on_pushButton_2_clicked()
{
    this->hide();
    pzhuce->show();
}

void Widget::zhuce(std::string username, std::string passwd, bool sex)
{
    QMSG msg;
    msg.type = 1;

    //定义一个注册结构体
    user rn(username.c_str(),passwd.c_str(),sex);
    memcpy(msg.buf,&rn,sizeof(rn));//把注册结构体装入传输结构体

    client->write((char*)&msg,sizeof(msg));
}

void Widget::dofanhui()
{
    this->show();
}
