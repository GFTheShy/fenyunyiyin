#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QTcpServer>
#include "qmsg.h"
#include "SqliteOperator.h"
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
private slots:
    void serverfunc();  //服务器响应客户端连接
    void readfunc();  //服务器响应客户端的消息
    void quitfunc();  //服务器响应客户端退出
private:
    Ui::Widget *ui;
    QTcpServer *server;
    QTcpSocket *client;
    SqliteOperator * sqliteOP;
};
#endif // WIDGET_H
