#ifndef CHOOSE_H
#define CHOOSE_H

#include <QMovie>
#include <QWidget>
#include "weather.h"
#include "music.h"
#include "five.h"

namespace Ui {
class choose;
}

class choose : public QWidget
{
    Q_OBJECT

public:
    explicit choose(QWidget *parent = nullptr);
    ~choose();
signals:
    void fanhui();  //返回信号
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void dofanhui();  //返回到该界面

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::choose *ui;
    QMovie *movie;
    weather *pwea;  //天气预报界面
    music *pmus;  //音乐界面
    five *pfive;  //五子棋界面
};

#endif // CHOOSE_H
