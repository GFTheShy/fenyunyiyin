#ifndef MUSIC_H
#define MUSIC_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QStandardItemModel>
#include <QMovie>
#include <QList>
#include <QMap>
namespace Ui {
class music;
}

struct song
{
    QString songname;   //歌曲名
    QString singer;     //歌手
    QString album_name; //专辑
    int duration2;      //时间
    QString music_id;       //音乐id
};

class music : public QWidget
{
    Q_OBJECT

public:
    explicit music(QWidget *parent = nullptr);
    ~music();
signals:
    void fanhui();
private slots:
    void on_pushButton_6_clicked();
    void do_network(QNetworkReply *reply);

    void on_pushButton_clicked();

    void on_pushButton_4_clicked();

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_tableView_clicked(const QModelIndex &index);

    void on_pushButton_5_clicked();

    void on_pushButton_3_clicked();

    void updateTableViewSelection(int currentIndex); //更新选中的界面显示

    void slot_positionChanged(qint64 position);
    void slot_durationChanged(qint64 duration);

    void on_horizontalSlider_2_actionTriggered(int action);

    void on_horizontalSlider_2_sliderReleased();

    void on_pushButton_2_clicked();

    void on_horizontalSlider_sliderReleased();

    void on_horizontalSlider_valueChanged(int value);

    void onStateChanged(const QMediaContent &media);  //响应播放加载歌词

    void updateLyricsForNewSong();

    void on_tableView_2_doubleClicked(const QModelIndex &index);

private:
    Ui::music *ui;
    QMovie *movie;
    QNetworkReply *reply;
    QString url;//网站
    QNetworkAccessManager *manager; //发送请求
    void jiexijsonNew(QByteArray data);  //解析得到的json
    void jiexiGeci(QByteArray data);  //解析歌词的json对象
    QList<song> songs;  //存放歌曲
    QStandardItemModel *model;
    QList<QStandardItem*> rowItems;
    void updeteTable();  //更新表格数据,播放列表

    QMediaPlaylist * playlist;
    QMediaPlayer * player;

    bool run;  //确定歌曲是否播放的图标
    int mnum=0;  //歌曲播放的，即给playlist的参数的第几首歌
    enum way  //播放方式
    {
        shunxu = 0,  //顺序播放
        suiji,   //随机播放
        danqu    //单曲循环
    };
    //播放方式默认为顺序播放
    int playway = shunxu;

    QMap<qint64, QString> lyricsMap; // 存储时间戳和歌词行
    void parseLyrics(const QString& lyricsContent); // 解析歌词函数
    void updateLyrics(qint64 position); // 根据播放进度更新歌词

    QString music_gc;  //歌词
    //解决有时候切换歌词时没有更新歌词的bug

    //历史播放列表功能
    QStandardItemModel *historyModel;  // 用于存储历史播放列表
    QMediaPlaylist *historyPlaylist;  // 用于存放历史播放列表
    //添加到历史播放记录
    void addToHistory(const song &playedSong);

    QMediaPlaylist *NowPlaylist;  // 当前播放列表
    QList<song> historySongs;  //主要是记录id，对应列表
};

#endif // MUSIC_H
