#include "choose.h"
#include "ui_choose.h"

#include <QLabel>

choose::choose(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::choose)
{
    // 去除窗口边框
    setWindowFlags(Qt::FramelessWindowHint);

    // 使用QLabel显示GIF
    QLabel *label = new QLabel(this);
    label->resize(500, 500);
    label->move(0, 0);

    // 加载并播放GIF
    movie = new QMovie(":/ui/菜单背景.gif");
    label->setMovie(movie);
    label->setScaledContents(true);
    movie->start();

    ui->setupUi(this);
    //天气预报
    pwea = new weather();
    connect(pwea,SIGNAL(fanhui()),this,SLOT(dofanhui()));

    //音乐播放器
    pmus = new music();
    connect(pmus,SIGNAL(fanhui()),this,SLOT(dofanhui()));

    //五子棋
    pfive = new five();
    connect(pfive,SIGNAL(fanhui()),this,SLOT(dofanhui()));

}

choose::~choose()
{
    delete ui;
}

void choose::on_pushButton_clicked()
{
    this->hide();
    fanhui();
}

void choose::on_pushButton_2_clicked()
{
    this->hide();
    pwea->show();
}

void choose::dofanhui()
{
    this->show();
}

void choose::on_pushButton_3_clicked()
{
    this->hide();
    pmus->show();
}

void choose::on_pushButton_4_clicked()
{
    this->hide();
    pfive->show();
}
