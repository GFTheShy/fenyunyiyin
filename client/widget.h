#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QString>
#include <QMovie>
#include <QPainter>
#include <QBitmap>
#include "qmsg.h"
#include "form.h"
#include "choose.h"
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    QBitmap createEllipticalMask() const;
protected:
    void contextMenuEvent(QContextMenuEvent *event);
private slots:
    void doconnected();  //连接，发信息
    void on_pushButton_clicked();
    void closeQuestion();
    void onReadyRead();  //收消息
    void on_pushButton_2_clicked();
    void zhuce(std::string username,std::string passwd,bool sex);  //注册

    void dofanhui();  //返回界面
private:
    Ui::Widget *ui;
    QMovie *movie;
    QTcpSocket *client;
    Form *pzhuce; //注册界面
    choose *pmenu;  //菜单界面
};
#endif // WIDGET_H
