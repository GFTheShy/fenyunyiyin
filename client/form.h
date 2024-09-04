#ifndef FORM_H
#define FORM_H

#include <QPainter>
#include <QWidget>

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();
signals:
    void fanhui();  //返回信号
    void user_zhuce(std::string username,std::string passwd,bool sex); //注册信号
private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::Form *ui;
    void paintEvent(QPaintEvent *event) override;

};

#endif // FORM_H
