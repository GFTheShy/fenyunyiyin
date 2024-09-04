#ifndef FIVE_H
#define FIVE_H

#include <QWidget>
#include <QPaintEvent>
#include <QTimer>
#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
#include <QPainterPath>
#include <QPen>
#include <QColor>
#include <QBrush>
#include <QMessageBox>
#include <QPushButton>
#include <QButtonGroup>
#include <QPalette>
#include <QtDebug>
#include <QLabel>
#include <bits/stdc++.h>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <iostream>
using namespace std;

namespace Ui {
class five;
}

class five : public QWidget
{
    Q_OBJECT

public:
    explicit five(QWidget *parent = nullptr);
    ~five();
signals:
    void fanhui();  //返回菜单界面
private:
    Ui::five *ui;
    int chessboard[15][15];  //棋盘 15 * 15
    int player; //玩家（1第一个玩家，2表示第二个玩家）
    int moveX,moveY; //控制鼠标move操作
    int currentX,currentY; //上一次落点位置
    bool lock; //棋盘锁
    int flags;  //游戏状态，0表示按钮为开始键，1为结束
    int game_model; //游戏模式：1 ：PVC  ，0 ：PVP
    QPushButton *button;
    QButtonGroup *selectModelGroup;
    QString strWinner;  //获胜者
    stack<QPoint> stak;  //悔棋操作

    //棋盘
    int jianxi = 55;  //间距
    int kuan = jianxi*14;  //棋盘长度，间距的14被
    //背景音乐
    QMediaPlayer *music;
    QMediaPlaylist * musiclist;
public:
    //更新棋盘
    void update_chessboard(int x,int y);

    //申明8个方向
    QPoint m_dir[8] = {
        QPoint(0,-1),  //上
        QPoint(1,-1),  //右上
        QPoint(1,0),  //右
        QPoint(1,1),  //右下
        QPoint(0,1),  //下
        QPoint(-1,1),  //左下
        QPoint(-1,0),  //左
        QPoint(-1,-1),  //左上
    };

    //dir表示方向，offset位移的棋子类型
    int getPointAt(QPoint p,int dir,int offset);

    //判断AI/用户，谁赢
    void is_Someone_Win(int x,int y);

    //是否五子连珠
    int isWin(QPoint p);

    //人下棋，重载的鼠标按下，获得位置
    void person_time(QMouseEvent *e);

    //AI下棋
    void ai_time();

    //AI 核心 算分
    int Assess(QPoint now,int me);

    //绘图事件
    void paintEvent(QPaintEvent *event);

    //鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event);

    //鼠标按压事件
    void mousePressEvent(QMouseEvent *event);

public slots:
    void operat();
    void SelectRadio();  //选择模式
    void back();
private slots:
    void on_pushButton_clicked();
};

#endif // FIVE_H
