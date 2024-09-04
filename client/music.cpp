#include "music.h"
#include "ui_music.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
music::music(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::music)
{
    // 去除窗口边框
    setWindowFlags(Qt::FramelessWindowHint);

    // 使用QLabel显示GIF
    QLabel *label = new QLabel(this);
    label->resize(804, 640);
    label->move(0, 0);

    // 加载并播放GIF
    movie = new QMovie(":/ui/音乐播放器.gif");
    label->setMovie(movie);
    label->setScaledContents(true);
    movie->start();

    ui->setupUi(this);

    //设置播放图标
    ui->pushButton_4->setIcon(QIcon(":/image/zt.png"));
    run = true;

    //设置顺序播放
    ui->pushButton_2->setIcon(QIcon(":/image/顺序循环111.png"));

    manager = new QNetworkAccessManager(this);

    // 连接 finished 信号到 do_network 槽函数
    connect(manager, &QNetworkAccessManager::finished, this, &music::do_network);

    // 创建QStandardItemModel
    model = new QStandardItemModel(0, 4, this); // 0行，4列

    // 设置列标题
    model->setHeaderData(0, Qt::Horizontal, "歌名");
    model->setHeaderData(1, Qt::Horizontal, "歌手");
    model->setHeaderData(2, Qt::Horizontal, "专辑");
    model->setHeaderData(3, Qt::Horizontal, "时长");

    // 将模型设置给QTableView
    ui->tableView->setModel(model);
    //关闭表头
    ui->tableView->horizontalHeader()->setVisible(false);
//    //表头透明
//    QHeaderView * pHeaderView = ui->tableView->horizontalHeader();
//    pHeaderView->setStyleSheet("QHeaderView::section{background-color:transparent; border: 1px solid #000;}");

//    QHeaderView * pHeaderView2 = ui->tableView_2->horizontalHeader();
//    pHeaderView2->setStyleSheet("QHeaderView::section{background-color:transparent; border: 1px solid #000;}");


    playlist = new QMediaPlaylist;

    player = new QMediaPlayer;

    NowPlaylist = playlist;  //设置当前播放列表
    player->setPlaylist(NowPlaylist);

    //顺序循环播放
    playlist->setPlaybackMode(QMediaPlaylist::Loop);

    //设置音量，对应的滑条
    ui->horizontalSlider->setMinimum(0);
    ui->horizontalSlider->setMaximum(100);
    ui->horizontalSlider->setValue(50); // 初始音量设置为中等

    // 初始化历史播放列表模型
    historyModel = new QStandardItemModel(0, 4, this); // 假设和主播放列表相同，有4列
    historyModel->setHeaderData(0, Qt::Horizontal, "歌名");
    historyModel->setHeaderData(1, Qt::Horizontal, "歌手");
    historyModel->setHeaderData(2, Qt::Horizontal, "专辑");
    historyModel->setHeaderData(3, Qt::Horizontal, "时长");

    // 设置历史播放列表的模型到 tableView_2
    ui->tableView_2->setModel(historyModel);
    // 初始化历史播放列表
    historyPlaylist = new QMediaPlaylist(this);

    // 连接历史播放列表的双击事件
    connect(ui->tableView_2, &QTableView::doubleClicked, this, &music::on_tableView_2_doubleClicked);

    // 将滑动条的valueChanged信号连接到媒体播放器的setVolume槽
    connect(ui->horizontalSlider, &QSlider::valueChanged, player, &QMediaPlayer::setVolume);

    connect(playlist, &QMediaPlaylist::currentIndexChanged, this, &music::updateTableViewSelection);

    connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(slot_positionChanged(qint64)));
    connect(player,SIGNAL(durationChanged(qint64)),this,SLOT(slot_durationChanged(qint64)));

    connect(player, &QMediaPlayer::currentMediaChanged, this, &music::onStateChanged);

    connect(player, &QMediaPlayer::positionChanged, this, &music::updateLyrics);

    connect(playlist, &QMediaPlaylist::currentIndexChanged, this, &music::updateLyricsForNewSong);

}

music::~music()
{
    delete ui;
}

void music::on_pushButton_6_clicked()
{
    this->hide();
    fanhui();
}

void music::do_network(QNetworkReply *reply)
{
    QVariant ret = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
//    qDebug() << "状态码:" << ret.toInt();
//    qDebug() <<url<<"=========访问的网站============="<<endl;

    QByteArray data = reply->readAll();
    if(ret == 200)//200说明页面访问成功
    {
        if(url.contains("api/song/lyric"))
        {
//            qDebug()<<"歌词解析"<<endl;
            jiexiGeci(data);  //得到歌词的解析
            updateLyrics(player->position()); // 在解析歌词后更新歌词
        }
        else
        {
//            qDebug()<<"歌单解析"<<endl;
            jiexijsonNew(data);  //调用解析
        }
    }
    else qDebug()<<"因为状态码不是200，所以没有获得数据,ret = "<<ret<<endl;
    reply->deleteLater(); // 清理reply对象
}

void music::jiexijsonNew(QByteArray data)
{
    songs.clear();  //清空链表

    QJsonParseError json_error;//JSON解析错误对象
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data, &json_error);
    song tsong;  //临时歌曲
    if(json_error.error == QJsonParseError::NoError)
    {
        if(parse_doucment.isObject())
        {
            QJsonObject rootobj = parse_doucment.object();
            if(rootobj.contains("result") && rootobj["result"].isObject())
            {
                QJsonObject temp = rootobj["result"].toObject();
                if(temp.contains("songs") && temp["songs"].isArray())
                {
                    QJsonArray arr = temp["songs"].toArray();
                    for(QJsonValue val:arr)
                    {
                        tsong.singer.clear();  // 在解析每首歌之前清空singer

                        QJsonObject vobj = val.toObject();

                        tsong.songname = vobj["name"].toString();
                        tsong.music_id = QString::number(vobj["id"].toInt());
                        tsong.duration2 = vobj["duration"].toInt();
                        if(vobj.contains("artists") && vobj["artists"].isArray())
                        {
                            QJsonArray t1 = vobj["artists"].toArray();
                            for(QJsonValue val:t1)
                            {
                                QJsonObject tobj = val.toObject();
                                tsong.singer += tobj["name"].toString() + "/";
                            }
                        }
                        if(vobj.contains("album") && vobj["album"].isObject())
                        {
                            QJsonObject t2 = vobj["album"].toObject();
                            tsong.album_name = t2["name"].toString();
                        }
                        songs.push_back(tsong);  //插入链表
                    }
//                    //调试
//                    for(int i=0;i<10;i++)
//                    {
//                        qDebug()<<songs[i].singer<<"=="<<songs[i].music_id<<"=="<<songs[i].songname
//                              <<"=="<<songs[i].duration2<<"=="<<songs[i].album_name<<endl;
//                    }
                }
            }
        }
        updeteTable();
    }
}

void music::jiexiGeci(QByteArray data)
{
//    qDebug()<<"这是歌词！！！解析"<<endl;
    music_gc.clear();  //清空歌词
    QJsonParseError json_error;//JSON解析错误对象
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data, &json_error);

    if(json_error.error == QJsonParseError::NoError)
    {
//        qDebug()<<"11---"<<endl;
        if(parse_doucment.isObject())
        {
//             qDebug()<<"22---"<<endl;
            QJsonObject rootobj = parse_doucment.object();
            if(rootobj.contains("lrc") && rootobj["lrc"].isObject())
            {
//                 qDebug()<<"33---"<<endl;
                QJsonObject temp = rootobj["lrc"].toObject();
                if(temp.contains("lyric") && temp["lyric"].isString())
                    music_gc = temp["lyric"].toString();
//                qDebug()<<"00000000000"<<music_gc<<endl;
            }
        }
    }
//    qDebug()<<music_gc<<endl;
    parseLyrics(music_gc);
    //更新歌词
    updateLyrics(player->position());
}

void music::updeteTable()
{
    // 清空播放列表
    playlist->clear();  // 清空播放列表

    // 遍历新的歌曲列表并添加到模型中
    for (int i = 0; i < songs.size(); ++i) {
        // 构建歌曲播放链接
        QString play_url = QString("http://music.163.com/song/media/outer/url?id=%1.mp3").arg(songs[i].music_id);

        playlist->addMedia(QUrl(play_url)); // 将歌曲添加到播放列表

        // 更新表格
        int num = songs[i].duration2 / 1000;
        QString str(QString::number(num / 60) + ":" + QString::number(num % 60));
        QStandardItem *item1 = new QStandardItem(songs[i].songname);
        QStandardItem *item2 = new QStandardItem(songs[i].singer);
        QStandardItem *item3 = new QStandardItem(songs[i].album_name);
        QStandardItem *item4 = new QStandardItem(str);

        // 检查是否需要更新现有行，或者插入新行
        if (i < model->rowCount()) {
            model->setItem(i, 0, item1);
            model->setItem(i, 1, item2);
            model->setItem(i, 2, item3);
            model->setItem(i, 3, item4);
        } else {
            model->appendRow(QList<QStandardItem*>{item1, item2, item3, item4});
        }
    }
}

void music::parseLyrics(const QString &lyricsContent)
{
    lyricsMap.clear(); // 清空之前的歌词

    QStringList lines = lyricsContent.split('\n');
    QRegularExpression re("\\[(\\d+):(\\d+\\.\\d+)\\]");
    foreach (const QString& line, lines) {
        QRegularExpressionMatch match = re.match(line);
        if (match.hasMatch()) {
            // 提取分钟和秒数
            int minutes = match.captured(1).toInt();
            float seconds = match.captured(2).toFloat();
            qint64 timestamp = static_cast<qint64>((minutes * 60 + seconds) * 1000); // 转换为毫秒

            // 提取歌词部分
            QString lyric = line.mid(match.capturedLength()).trimmed();
            if (!lyric.isEmpty()) {
                lyricsMap[timestamp] = lyric;
            }
        }
    }
}

void music::updateLyrics(qint64 position)
{
    if (lyricsMap.isEmpty()) {
        return;
    }

    QString lyricsDisplay;  // 用于 QTextBrowser 显示的歌词文本
    int linesBefore = 2;    // 当前歌词前显示的行数
    int linesAfter = 4;     // 当前歌词后显示的行数

    // 获取歌词列表
    QList<QPair<qint64, QString>> lyricsList;
    for (auto i = lyricsMap.begin(); i != lyricsMap.end(); ++i) {
        lyricsList.append(qMakePair(i.key(), i.value()));
    }

    // 找到当前播放时间的歌词索引
    auto it = lyricsMap.lowerBound(position);
    if (it != lyricsMap.begin() && (it == lyricsMap.end() || it.key() > position)) {
        --it;
    }

    int currentIndex = lyricsList.indexOf(qMakePair(it.key(), it.value()));
    if (currentIndex == -1) {
        return;
    }

    // 构造歌词显示文本
    lyricsDisplay += "<div style='text-align: center; vertical-align: middle; display: table; height: 100%; width: 100%;'>";
    lyricsDisplay += "<div style='display: table-cell; vertical-align: middle; text-align: center;'>";

    for (int i = currentIndex - linesBefore; i < currentIndex; ++i) {
        if (i >= 0) {
            lyricsDisplay += "<p style='color: gray; font-size: 18px;'>" + lyricsList[i].second + "</p>";
        }
    }

    // 高亮显示当前歌词
    lyricsDisplay += "<p style='color: green; font-size: 24px;'>" + lyricsList[currentIndex].second + "</p>";

    // 显示未来歌词
    for (int i = currentIndex + 1; i <= currentIndex + linesAfter; ++i) {
        if (i < lyricsList.size()) {
            lyricsDisplay += "<p style='color: black; font-size: 18px;'>" + lyricsList[i].second + "</p>";
        }
    }

    lyricsDisplay += "</div></div>";

    ui->textBrowser->setHtml(lyricsDisplay);

    // 自动滚动到当前歌词
    QTextCursor cursor = ui->textBrowser->textCursor();
    cursor.movePosition(QTextCursor::Start);
    for (int i = 0; i < currentIndex - linesBefore && i < ui->textBrowser->document()->lineCount(); ++i) {
        cursor.movePosition(QTextCursor::Down);
    }
    ui->textBrowser->setTextCursor(cursor);
    ui->textBrowser->ensureCursorVisible();
}

void music::updateLyricsForNewSong()
{
//    qDebug() << "调用了切换歌曲的加载歌词函数" << endl;
    // 确保当前歌曲的歌词已加载
    if (NowPlaylist->currentIndex() >= 0 && NowPlaylist->currentIndex() < model->rowCount())
    {
//        qDebug() << "我现在更新歌词了" << endl;
        // 使用当前歌曲的ID请求新的歌词
        QString songId;
        if(NowPlaylist == playlist)
        {
//            qDebug()<<"niubi牛逼放的更新歌词"<<endl;
            songId = songs[playlist->currentIndex()].music_id;
        }
        else if(NowPlaylist == historyPlaylist)
        {
//            qDebug()<<"历史播放的更新歌词"<<endl;
            songId = historySongs[historyPlaylist->currentIndex()].music_id;
//            qDebug()<<songId<<"=============================="<<endl;
        }
        else
        {
            songId = "";
        }
        url = QString("http://music.163.com/api/song/lyric?id=%1&lv=1&kv=1&tv=-1").arg(songId);
        manager->get(QNetworkRequest(QUrl(url)));
    }
}

void music::on_tableView_2_doubleClicked(const QModelIndex &index)
{
    int row = index.row();
    if (row < 0 || row >= historyPlaylist->mediaCount()) {
        qDebug() << "无效的历史播放索引";
        return;
    }

    // 切换到历史播放列表并播放
    NowPlaylist = historyPlaylist;
    player->setPlaylist(historyPlaylist);
    historyPlaylist->setCurrentIndex(row);
    player->play();
    // 更新歌词
    updateLyricsForNewSong();

    ui->pushButton_4->setIcon(QIcon(":/image/好看的播放.png")); // 更新播放按钮图标为播放状态
}

void music::on_pushButton_clicked()
{
    QString key = ui->lineEdit->text();
    url = QString("http://music.163.com/api/search/get/web?csrf_token=hlpretag=&hlposttag=&s=%1&type=1&offset=0&total=true&limit=10").arg(key);
    manager->get(QNetworkRequest(QUrl(url)));
}

void music::on_pushButton_4_clicked()
{
    //判断图标
    if (run == true)
    {
        //播放歌曲
        player->play();
        // 切换到另一个图标
        ui->pushButton_4->setIcon(QIcon(":/image/好看的播放.png"));
        run = false;
    }
    else
    {
        //暂停播放
        player->pause();
        // 切换回初始图标
        ui->pushButton_4->setIcon(QIcon(":/image/zt.png"));
        run = true;
    }
}

void music::on_tableView_doubleClicked(const QModelIndex &index)
{
//    qDebug()<<"双击播放了！！"<<endl;
    mnum = index.row();
    if(mnum < 0 || mnum >= songs.size())
    {
//        qDebug()<<"超出范围了"<<endl;
        return;
    }
    playlist->setCurrentIndex(mnum);//
    ui->pushButton_4->setIcon(QIcon(":/image/好看的播放.png"));

    NowPlaylist = playlist;
    player->setPlaylist(NowPlaylist);

    player->play();

    // 添加到历史播放列表
    addToHistory(songs[mnum]);
}

void music::on_tableView_clicked(const QModelIndex &index)
{
    mnum = index.row();  //设置播放
//    qDebug()<<mnum<<"===1111111"<<endl;
    //切换歌曲
    playlist->setCurrentIndex(mnum);//双击的行数就是播放列表中歌曲的下标
}

void music::on_pushButton_5_clicked()
{
//    qDebug()<<mnum<<endl;

    mnum = playlist->currentIndex();
    playlist->next(); //下一首
    // 更新歌词
    updateLyricsForNewSong();

    // 添加到历史播放列表
    addToHistory(songs[mnum]);
}

void music::on_pushButton_3_clicked()
{
    mnum = playlist->currentIndex();
    playlist->previous();  //上一首
    // 更新歌词
    updateLyricsForNewSong();

    // 添加到历史播放列表
    addToHistory(songs[mnum]);
}

void music::updateTableViewSelection(int currentIndex)
{
    // 确保当前索引有效
    if (currentIndex < 0 || currentIndex >= model->rowCount())
        return;

    // 获取QTableView的选择模型
    QItemSelectionModel *selectionModel = ui->tableView->selectionModel();

    // 取消所有选择
    selectionModel->clearSelection();

    // 选中当前播放的歌曲
    QModelIndex index = model->index(currentIndex, 0); // 选中当前行的第一列
    selectionModel->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);

    // 滚动到当前播放的歌曲
    ui->tableView->scrollTo(index);
}

void music::slot_positionChanged(qint64 position)
{
//        qDebug()<<position<<endl;
    // 更新已经播放时间
    int seconds = position / 1000;
    int minutes = seconds / 60;
    seconds %= 60;

    // 使用QString::arg()来格式化字符串，并保留两位数
    QString str = QString("%1:%2")
                    .arg(minutes, 2, 10, QChar('0'))  // 两位数的分钟，不足时前面补0
                    .arg(seconds, 2, 10, QChar('0')); // 两位数的秒，不足时前面补0
    ui->label_7->setText(str);
    //滑动进度条
    ui->horizontalSlider_2->setValue(position);
}

void music::slot_durationChanged(qint64 duration)
{
//    qDebug()<<"开始播放一首新歌，总时长为："<<duration<<endl;
    // 总时长
    int seconds = duration / 1000;
    int minutes = seconds / 60;
    seconds %= 60;

    // 使用QString::arg()来格式化字符串，并保留两位数
    QString str = QString("%1:%2")
                    .arg(minutes, 2, 10, QChar('0'))  // 两位数的分钟，不足时前面补0
                    .arg(seconds, 2, 10, QChar('0')); // 两位数的秒，不足时前面补0
    ui->label_4->setText(str);
    //进度条最大值
     ui->horizontalSlider_2->setRange(0,duration);
}

void music::on_horizontalSlider_2_actionTriggered(int action)
{
}

void music::on_horizontalSlider_2_sliderReleased()
{
    //拖动进度条歌曲进度也会变
    int ret = ui->horizontalSlider_2->value();
    player->setPosition(ret);
}

void music::on_pushButton_2_clicked()
{
//    QMediaPlaylist::CurrentItemOnce：当前曲目只播放一次。
//    QMediaPlaylist::CurrentItemInLoop：当前曲目循环播放。
//    QMediaPlaylist::Sequential：从当前曲目开始顺序播放至列表结尾，然后结束播放。
//    QMediaPlaylist::Loop：列表循环播放。
//    QMediaPlaylist::Random：随机播放。
    //切换播放方式
    if(playway == shunxu)
    {
        playway = suiji;
        ui->pushButton_2->setIcon(QIcon(":/image/随机循环111.png"));
        playlist->setPlaybackMode(QMediaPlaylist::Random);
    }
    else if(playway == suiji)
    {
        playway = danqu;
        ui->pushButton_2->setIcon(QIcon(":/image/单曲循环111.png"));
        playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    }
    else
    {
        playway = shunxu;
        ui->pushButton_2->setIcon(QIcon(":/image/顺序循环111.png"));
        playlist->setPlaybackMode(QMediaPlaylist::Loop);
    }
}

void music::on_horizontalSlider_sliderReleased()
{
}

void music::on_horizontalSlider_valueChanged(int value)
{
//    qDebug()<<value<<endl;
    ui->label_5->setText(QString::number(value));
}

void music::onStateChanged(const QMediaContent &media)
{
//    qDebug()<<"这是加载歌词的槽函数！！！！喂！！！"<<endl;
    QString str = songs[mnum].music_id;

//    qDebug()<<str<<"这是我的音乐id！！"<<endl;
    url = QString("http://music.163.com/api/song/lyric?id=%1&lv=1&kv=1&tv=-1").arg(str);
//    qDebug()<<url<<"这是我的网站"<<endl;
    manager->get(QNetworkRequest(QUrl(url)));

    // 更新歌词
    updateLyricsForNewSong();
}

void music::addToHistory(const song &playedSong)
{
    // 检查是否已经在历史中存在
    for (int row = 0; row < historyModel->rowCount(); ++row) {
        if (historyModel->item(row, 0)->text() == playedSong.songname &&
            historyModel->item(row, 1)->text() == playedSong.singer) {
            return;  // 如果已存在，则不添加
        }
    }

    // 构建歌曲的项并添加到历史模型中
    QStandardItem *item1 = new QStandardItem(playedSong.songname);
    QStandardItem *item2 = new QStandardItem(playedSong.singer);
    QStandardItem *item3 = new QStandardItem(playedSong.album_name);

    int num = playedSong.duration2 / 1000;
    QString durationStr = QString::number(num / 60) + ":" + QString::number(num % 60);
    QStandardItem *item4 = new QStandardItem(durationStr);

    historyModel->appendRow(QList<QStandardItem*>{item1, item2, item3, item4});

    // 构建歌曲播放链接
    QString play_url = QString("http://music.163.com/song/media/outer/url?id=%1.mp3").arg(playedSong.music_id);

    historyPlaylist->addMedia(QUrl(play_url)); // 将歌曲添加到历史播放列表
    historySongs.push_back(playedSong);
}
