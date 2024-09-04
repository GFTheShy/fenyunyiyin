#include "form.h"
#include "ui_form.h"

#include <QMessageBox>

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    // 去除窗口边框
    setWindowFlags(Qt::FramelessWindowHint);
}

Form::~Form()
{
    delete ui;
}

void Form::on_pushButton_2_clicked()
{
    this->hide();
    fanhui();
}

void Form::paintEvent(QPaintEvent *event)
{
    QPixmap backgroundPixmap;
    backgroundPixmap.load(":/msg/新注册背景.png");
    QPainter painter(this);
    // 根据窗口大小调整图片尺寸，保持图片比例
    QPixmap scaledPixmap = backgroundPixmap.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    // 将调整后的图片设置为窗口背景
    painter.drawPixmap(this->rect(), scaledPixmap);
}

void Form::on_pushButton_clicked()
{

    QString str = ui->username->text();//名字
    std::string str1 = str.toStdString();

    QString str_pw = ui->passwd->text();//密码
    std::string str_pw1 = str_pw.toStdString();//转成标准的字符串才能获得里面的char *

    QString str_pw2 = ui->passwd_2->text();//密码
    std::string str_pw3 = str_pw2.toStdString();//转成标准的字符串才能获得里面的char
    bool sex = false;
    if(ui->radioButton->isChecked())
    {
        sex = true;
    }
    if(str_pw1 == str_pw3)
    {
        //发送注册信号
        user_zhuce(str1,str_pw1,sex);
    }
    else
    {
        QMessageBox messageBox;
        messageBox.setIconPixmap(QPixmap(":/msg/错误.png"));
        messageBox.setText("<font color='red'>两次输入的密码不一致，请重新输入！</font>");
        messageBox.setWindowTitle("错误");
        messageBox.setStandardButtons(QMessageBox::Ok);
        messageBox.exec();
    }
}
