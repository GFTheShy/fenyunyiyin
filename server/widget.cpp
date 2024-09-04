#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QString>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    server = new QTcpServer(this);
    server->listen(QHostAddress::Any,9000);
    connect(server,SIGNAL(newConnection()),this,SLOT(serverfunc()));

    sqliteOP = new SqliteOperator;//创建数据连接
    sqliteOP->openDb();

//    sqliteOP->normalExec("create table user(name varchar(128),password varchar(128),sex int)");
}

Widget::~Widget()
{
    delete ui;
    sqliteOP->closeDb();
}

void Widget::serverfunc()
{
    client = server->nextPendingConnection();
    connect(client, &QTcpSocket::readyRead, this, &Widget::readfunc);

    connect(client, &QTcpSocket::disconnected, this, &Widget::quitfunc);
}

void Widget::readfunc()
{
    if (client) {
       QMSG msg;
       int len = client->read((char*)&msg,sizeof(msg));//读取反过来的所有数据
       qDebug()<<"一共收到"<<len<<"个字节";

       if(msg.type == 1)//注册
       {
           user * rn = (user*)msg.buf;
           QString sql = QString("insert into user values('%1','%2', %3)").arg(rn->name).arg(rn->password).arg(rn->sex);
           sqliteOP->normalExec(sql);
       }
       else if(msg.type == 2)//登录
       {
           //这个是用注册结构体发来的登录信息
           user * rn = (user*)msg.buf;
           qDebug()<<rn->name<<rn->password;

           QSqlQuery sqlQuery;//这个结构体用来存放查询的结果
           sqliteOP->normalQuery(sqlQuery, QString("select * from user where name='%1' and password='%2'").arg(rn->name).arg(rn->password));
           if(sqlQuery.next())
           {
               //用户登录成功
               //给客户端回复GET
               char buf[128] = "get";
               qDebug()<<buf<<endl;
               client->write(buf,128);
           }
           else
           {
               char buf[128] = "not";
               qDebug()<<buf<<endl;
               client->write(buf,128);
           }
       }

    }
}

void Widget::quitfunc()
{
    qDebug()<<"退出了！！！！！！！"<<endl;
}

