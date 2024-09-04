#include "five.h"
#include "ui_five.h"

five::five(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::five)
{
    ui->setupUi(this);

    // 去除窗口边框
    setWindowFlags(Qt::FramelessWindowHint);

    connect(ui->pvp,SIGNAL(clicked()),this,SLOT(SelectRadio()));
    connect(ui->pvc,SIGNAL(clicked()),this,SLOT(SelectRadio()));

    //初始化玩家顺序
    player = 1;
    //设置默认为人机对战
    ui->pvc->setChecked(true);
    game_model = 1;

    //开始游戏和悔棋
    connect(ui->begin,SIGNAL(clicked()),this,SLOT(operat()));
    connect(ui->huiqi,SIGNAL(clicked()),this,SLOT(back()));

    //悔棋操作初始化，清空栈
    while(!stak.empty())
    {
        stak.pop();
    }
    //开启鼠标监听操作
    setMouseTracking(true);
    lock = true;
    memset(chessboard,0,sizeof(chessboard));  //初始化棋盘

    //背景音乐
    music = new QMediaPlayer(this);
    musiclist = new QMediaPlaylist;

    music->setPlaylist(musiclist);

    //顺序循环播放
    musiclist->setPlaybackMode(QMediaPlaylist::Loop);
    // 设置并播放音乐
    musiclist->addMedia(QUrl("http://music.163.com/song/media/outer/url?id=1831784095.mp3")); // 将歌曲添加到播放列表
}

five::~five()
{
    delete ui;
}

void five::update_chessboard(int x, int y)
{
    if(x >= 0 && x < 15 && y>= 0 && y < 15 && chessboard[x][y] == 0)
    {
        chessboard[x][y] = player;
        stak.push(QPoint(x,y)); //记录步数
        player = 3 - player;
    }
}

int five::getPointAt(QPoint p, int dir, int offset)
{
    int nx = p.x() + m_dir[dir].x() * offset;
    int ny = p.y() + m_dir[dir].y() * offset;
    if(nx >= 0 && nx < 15 && ny >= 0 && ny <15)
    {
        return chessboard[nx][ny];
    }
    return -1; //越界
}

void five::is_Someone_Win(int x, int y)
{
    int winner = isWin(QPoint(x,y));//得到获胜玩家的消息
    if(winner == 1)
    {
        QMessageBox::information(this,"游戏结束","黑方胜利！");
        lock = true;
        music->pause();
    }
    else if(winner == 2)
    {
        QMessageBox::information(this,"游戏结束","白方胜利！");
        lock = true;
        music->pause();
    }
}

int five::isWin(QPoint p)
{
    int currentPlayer = chessboard[p.x()][p.y()];  //当前玩家
    if(currentPlayer == 0) return 0; //空位

    for(int dir = 0;dir < 8;dir+=2)
    {
        int count = 1; //自己算一个
        //计算上下左右的连续数量
        for(int i = 1;i <= 4;i++)
        {
            if(getPointAt(p,dir,i) == currentPlayer) count++;
            else break;
        }

        if(count >= 5) return currentPlayer;
        else count = 1;

        //计算左上右上，左下右下的连续数量
        for(int i = 1;i <= 4;i++)
        {
            if(getPointAt(p,dir+1,i) == currentPlayer) count++;
            else break;
        }
        if(count >= 5) return currentPlayer;
    }
    return 0; //没有胜利
}

void five::person_time(QMouseEvent *e)
{
//    qDebug()<<"调用了玩家下棋"<<endl;
    //获得落子位置
    int x = e->pos().x() / jianxi;
    int y = e->pos().y() / jianxi;
//    qDebug()<<"下棋位置为："<<x<<","<<y<<endl;
    if(x >= 0 && x < 15 && y >= 0 && y < 15 && chessboard[x][y] == 0 && !lock)
    {
        qDebug()<<"当前下棋的玩家："<<player<<endl;
        update_chessboard(x,y);  //下棋
        is_Someone_Win(x,y);  //判断是否获胜
        if(!lock && game_model == 1)
        {
            ai_time();
        }
        else if(game_model == 0)
        {
            //不用写，下棋之后会自动切换玩家
        }
    }
}

void five::ai_time()
{
    if(lock || game_model == 0) return;  //pvp模式跳过

    int bestScore = -1;  //得分，判断哪个位置最好
    QPoint bestMove;  //ai落子

    //遍历可以落子的所有位置，得到最佳的落子位置
    for(int x = 0;x < 15;++x)
    {
        for(int y = 0;y < 15;++y)
        {
            if(chessboard[x][y] == 0)
            {
                QPoint p(x,y);
                int score = Assess(p,player);
                if(score > bestScore)
                {
                    bestScore = score;
                    bestMove = p;
                }
            }
        }
    }

    //落子
    update_chessboard(bestMove.x(),bestMove.y());
    is_Someone_Win(bestMove.x(),bestMove.y());
    update();
}

int five::Assess(QPoint now, int me)
{
    int score = 0;

    for(int dir = 0;dir < 8;dir += 2)
    {
        int count_me = 0,count_opponent = 0;  //当前玩家和对手玩家的连续棋子数量
        int empty_me = 0,empty_opponent = 0;  //当前玩家和对手遇到的第一个空位后的连续空位


        //me
        // 检查当前玩家在dir方向上的连续棋子
        for(int i = 1;i <= 4;i++)
        {
            if(getPointAt(now,dir,i) == me)
            {
                count_me++;
            }
            else if(getPointAt(now,dir,i) == 0)
            {
                empty_me++; // 发现空位，增加连续空位计数
                break;  // 遇到对手棋子，停止检查
            }
            else
            {
                break;
            }
        }

        for(int i = 1;i <= 4;i++)
        {
            if(getPointAt(now,dir+1,i) == me)
            {
                count_me++;
            }
            else if(getPointAt(now,dir+1,i) == 0)
            {
                empty_me++;
                break;
            }
            else
            {
                break;
            }
        }

        //对手
        for (int i = 1; i <= 4; i++)
        {
            if (getPointAt(now, dir, i) == 3 - me)
            {
                count_opponent++;
            }
            else if (getPointAt(now, dir, i) == 0)
            {
                empty_opponent++;
                break;
            }
            else
            {
                break;
            }
        }

        for (int i = 1; i <= 4; i++)
        {
            if (getPointAt(now, dir + 1, i) == 3 - me)
            {
                count_opponent++;
            }
            else if (getPointAt(now, dir + 1, i) == 0)
            {
                empty_opponent++;
                break;
            }
            else
            {
                break;
            }
        }

        // 评估局势并更新得分
        if(count_me == 4 && empty_me > 0) score += 100000;  //成五
        else if(count_me == 3 && empty_me > 0) score += 10000;  //活4
        else if(count_me == 2 && empty_me > 0) score += 1000;  //活3
        else if(count_me == 1 && empty_me > 0) score += 100;  //活2

        if(count_opponent == 4 && empty_opponent > 0) score += 50000;  //阻止对手成5
        else if(count_opponent == 3 && empty_opponent > 0) score += 5000;  //阻止对手成4
        else if(count_opponent == 2 && empty_opponent > 0) score += 500;  //阻止对手成3
        else if(count_opponent == 1 && empty_opponent > 0) score += 50;  //阻止对手成2
    }

    return score;
}

void five::paintEvent(QPaintEvent *event)
{
//    qDebug()<<"调用了绘图事件"<<endl;
    QPainter painter(this);
    QPen pen;
    pen.setColor(Qt::black);
    painter.setPen(pen);

    //画棋盘
    for(int i = 0;i<15;++i)
    {
//        qDebug()<<"画棋盘"<<endl;
        painter.drawLine(jianxi * i,0,jianxi * i,kuan);
        painter.drawLine(0,jianxi*i,kuan,jianxi*i);
    }

    //画棋子
    for(int x = 0;x < 15; x++)
    {
        for(int y = 0;y < 15;y++)
        {
//            qDebug()<<chessboard[x][y];
            if(chessboard[x][y] == 1)  //黑棋
            {
                painter.setBrush(Qt::black);
                painter.drawEllipse(jianxi * x + 5,jianxi * y + 5,jianxi - 10,jianxi - 10);
            }
            else if(chessboard[x][y] == 2)  //白棋
            {
                painter.setBrush(Qt::white);
                painter.drawEllipse(jianxi * x + 5,jianxi * y + 5,jianxi - 10,jianxi - 10);
            }
        }
    }
}

void five::mouseMoveEvent(QMouseEvent *event)
{
    moveX = event->pos().x();
    moveY = event->pos().y();
    update();
}

void five::mousePressEvent(QMouseEvent *event)
{
//    qDebug()<<"鼠标按下去了"<<endl;
    if(event->button() == Qt::LeftButton && !lock)
    {
//        qDebug()<<"下棋了"<<lock<<endl;
        person_time(event);
    }
}

void five::operat()
{
    lock = !lock;
    if(!lock)
    {
        QMessageBox::information(this,"提示","游戏开始");
        music->play();
    }
    else
    {
        QMessageBox::information(this,"提示","游戏暂停");
        music->pause();
    }
}

void five::SelectRadio()
{
    //游戏模式：1 ：PVC  ，0 ：PVP
    if(ui->pvp->isChecked())
    {
        game_model = 0;
    }
    else if(ui->pvc->isChecked())
    {
        game_model = 1;
    }

}

void five::back()
{
    if(stak.empty() || lock) return;

    QPoint lastMove = stak.top();
    stak.pop();
    chessboard[lastMove.x()][lastMove.y()] = 0;  //撤销上一步

    if(game_model == 1 && stak.empty())
    {
        QPoint aiMove = stak.top();
        stak.pop();
        chessboard[aiMove.x()][aiMove.y()] = 0;  //撤销ai的上一步
    }
    else if(game_model == 0)
    {
        player = 3 - player;  //切换玩家
    }

    update();
}

void five::on_pushButton_clicked()
{
    this->hide();
    fanhui();
}
