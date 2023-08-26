#include <QApplication>
#include <QWidget>
#include <QKeyEvent>
#include <QTimer>
#include <QPixmap>
#include <QLabel>
#include <QList>
#include <QIcon>
#include <QDebug>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QList>
#include <QRandomGenerator>
#include <QPushButton>
#include <QImage>
#include <QTextEdit>
#include <QString>
#include <QMediaPlayer>
#include <QUrl>
#include <QFile>

enum class GameState//状态机，枚举不同的游戏状态
{
    Start,
    InGame,
    Gameover
};
class Mainwindow:public QWidget
{

public:
    Mainwindow(QWidget *parent=nullptr):QWidget(parent),gameState(GameState::Start)
    {
        QPixmap backgroungImage(":/images/kunback.png");
        bird=new QLabel(this);
        background=new QLabel(this);
        background->setPixmap(backgroungImage);
        background->lower();

        this->setFixedSize(backgroungImage.width(),backgroungImage.height());
        this->setWindowTitle("flappy kunkun");
        this->setWindowIcon(QIcon(":/images/kunkun.png"));

        btn1=new QPushButton("开始游戏",this);
        btn1->setGeometry(width()/2-50,height()/2-25,100,50);
        connect(btn1,&QPushButton::clicked,this,&Mainwindow::update1);

        //缩放图片
        QPixmap birdImage(":/images/kunkun05.png");
        birdImage=birdImage.scaled(90,90,Qt::KeepAspectRatio);
        bird->setPixmap(birdImage);
        bird->raise();//让小鸟处于最上层
        bird->move(175,(height()-bird->height())/2);
        bird->hide();//隐藏kunkun

        QPixmap gameoutImage(":/images/gameover2.png");
        gameout=new QLabel(this);
        gameout->setPixmap(gameoutImage);
        gameout->move((width()-gameoutImage.width())/2,(height()-gameoutImage.height())/2);//图片居中
        gameout->raise();
        gameout->hide();//隐藏gameover图片

        btn2=new QPushButton("重新开始",this);
        btn2->setGeometry(gameout->x(),550,100,50);
        btn2->hide();//隐藏重新开始按钮
        connect(btn2,&QPushButton::clicked,this,&Mainwindow::update2);

        //篮球图片——障碍物
        topLanqiu=new QLabel(this);
        bottomLanqiu=new QLabel(this);
        QPixmap lanqiuImage(":/images/basketball01.png");
        lanqiuImage=lanqiuImage.scaled(200,200,Qt::KeepAspectRatio);
        topLanqiu->setPixmap(lanqiuImage);
        bottomLanqiu->setPixmap(lanqiuImage);
        topLanqiu->raise();
        bottomLanqiu->raise();
        topLanqiu->move(width(),0);
        bottomLanqiu->move(width(),height()/2+50);

        timer=new QTimer(this);
        connect(timer,&QTimer::timeout,this,&Mainwindow::rungame);
        timer->start(20);

        //分数
        scoreLabel = new QLabel(this);
        scoreLabel->setText("Current Score: 0");
        scoreLabel->setGeometry( 0, 0, 200, 30);
        scoreLabel->show();

        count=new QLabel(this);
        count->setGeometry(width()/2+30,550,100,50);
        count->hide();

        tips=new QLabel(this);
        //tips->setText("<html><body>""<p> tips:点击鼠标左右键都可以让坤坤起飞哟！"
        //              "<span style='color: red;'>坤坤和篮球都是矩阵哟，可不要被像素点欺骗了……</span></p>"
         //             "</body></html>");
        tips->setText("tips: 点击鼠标左右键都可以让坤坤起飞哟！");
        tips->setGeometry(0,height()-30,1200,30);

        Bgm.setMedia(QUrl::fromLocalFile("E:\\FFOutput\\zhiyinnitaimei.mp3"));
        Clicked.setMedia(QUrl::fromLocalFile("D:/download/kunshout.mp3"));
        Welcome.setMedia(QUrl::fromLocalFile("D:\\download\\zhendeshinia.mp3"));
        failure.setMedia(QUrl::fromLocalFile("D:\\download\\niganma.mp3"));
        Bgm.setVolume(5);
        Clicked.setVolume(100);
        Welcome.setVolume(50);
        stage_award.setMedia(QUrl::fromLocalFile("D:\\download\\libulihainikunge.mp3"));
        stage_award.setVolume(100);
        Welcome.play();

        birdVelocity = 0;
        gravity = 1;

        //选择模式界面
        btncommon=new QPushButton("普通模式",this);
        btnhell=new QPushButton("地狱模式",this);
        btncommon->setGeometry(btn1->x(),btn1->y(),100,50);
        btnhell->setGeometry(btncommon->x(),btncommon->y()+50,100,50);
        btncommon->hide();
        btnhell->hide();
        Choose=new QLabel("选择模式:",this);
        Choose->setGeometry(btn1->x(),0,100,50);
        Choose->hide();

        connect(btncommon,&QPushButton::clicked,this,&Mainwindow::startcommon);
        connect(btnhell,&QPushButton::clicked,this,&Mainwindow::starthell);

        hello=new QLabel(this);
        hello->setGeometry(btn1->x()-75,btn1->y()-100,300,50);
        hello->setText("<html><body>"
                       "<p style='font-size: 24px;'>Welcome To Flappy KUN!</p>"
                       "</body></html>");
        hello->show();

        loadHighScores(); // 加载排行榜数据
        scoreBoard = new QTextEdit(this);
        scoreBoard->setGeometry(300, 50, 200, 400);
        scoreBoard->hide();
        scoreBoard->setStyleSheet("background-color: rgba(0,0,0,0);");
        showScoreBtn = new QPushButton("排行榜", this);
        showScoreBtn->setGeometry(50, 600, 100, 50);
        showScoreBtn->setStyleSheet("background-color: rgba(0,0,0,0);");

        connect(showScoreBtn, &QPushButton::clicked, this, &Mainwindow::displayScoreBoard);

        closeScoreBoard=new QPushButton("关闭排行榜",this);
        closeScoreBoard->setGeometry(count->x()+200,count->y(),100,50);
        closeScoreBoard->hide();
        connect(closeScoreBoard,&QPushButton::clicked,this,&Mainwindow::CloseScore);

        closeGame=new QPushButton("退出游戏",this);
        closeGame->setGeometry(1300,0,100,50);
        closeGame->setStyleSheet("background-color: rgba(0,0,0,0);");
        closeGame->show();
        closeGame->raise();
        connect(closeGame,&QPushButton::clicked,this,&Mainwindow::close);

    }
    void rungame()
    {
        if(gameState==GameState::InGame)
        {
            MoveBirdAndPipe();
        }
        else if(gameState==GameState::Gameover)
        {
            return;
        }
    }
    void moveBirdDown()
    {   //下降函数
        birdVelocity += gravity;//速度增加，模拟重力加速度
        QPoint birdpos=bird->pos();//获取小鸟位置
        birdpos.setY(birdpos.y()+birdVelocity);//设置小鸟下降速度
        //判断小鸟是否下降到边界
        if (birdpos.y() > (this->height() - bird->height()))
        {
            gameover();
            return;
        }
        bird->move(birdpos);
    }
    void moveBirdUp()
    {
        if(gameState==GameState::Gameover)
            return;
        QPoint birdpos = bird->pos(); // 获取小鸟位置
        birdpos.setY(birdpos.y() - 60); // 上升
        // 确保小鸟不超出窗口顶部边界
        if (birdpos.y() < 0)
        {
            birdpos.setY(0);
        }
        bird->move(birdpos);
        Clicked.play();
    }
    void gameover()
    {
        Bgm.stop();
        failure.play();
        showScoreBtn->show();
        //timer->stop();
        gameState=GameState::Gameover;
        count->setText("Score: "+QString::number(scores));
        count->show();
        updateHighScores();
        ResetBird();
        ResetLanqiu();
        bird->hide();
        qDebug()<<"game over";

        gameout->show();
        btn2->show();
    }
    void MoveBirdAndPipe()
    {
        moveBirdDown();
        topLanqiu->move(topLanqiu->x()-sudu,topLanqiu->y());
        bottomLanqiu->move(bottomLanqiu->x()-sudu,bottomLanqiu->y());
        if(IsCrash())
        {
            gameover();
            return;
        }
        if(topLanqiu->x()<-225)
        {
            scores++;
            updatescore();
            ResetLanqiu();
        }
    }
    void ResetBird()
    {
        bird->move(175,250);
    }
    void ResetLanqiu()
    {   //重置篮球位置
        int windowHeight = height();
        int topHeight;
        topHeight=random.bounded(-50,windowHeight/2);
        qDebug()<<"topHeight: "<<topHeight;
        int bottomHeight = topHeight+425;
        topLanqiu->move(width(),topHeight);
        bottomLanqiu->move(width(),bottomHeight);
    }
    void update1()
    {
        btncommon->show();
        btnhell->show();
        Choose->show();
        showScoreBtn->hide();
        btn1->hide();
        hello->hide();
    }
    void startcommon()
    {
        btncommon->hide();
        btnhell->hide();
        Choose->hide();
        sudu=10;
        bird->show();
        Bgm.play();
        gameState=GameState::InGame;
    }
    void starthell()
    {
        btncommon->hide();
        btnhell->hide();
        Choose->hide();
        sudu=30;
        bird->show();
        Bgm.play();
        gameState=GameState::InGame;
    }
    void update2()
    {
        Welcome.play();
        Bgm.play();
        //gameState=GameState::InGame;
        scores=0;
        updatescore();
        gameout->hide();
        btn2->hide();
        count->hide();
        //bird->show();
        showScoreBtn->hide();
        btncommon->show();
        btnhell->show();
        Choose->show();
    }
    void updatescore()
    {
        scoreLabel->setText("Current Score: " + QString::number(scores));
        if(scores%5==0&&scores>0)
            stage_award.play();
    }
    bool IsCrash()
    {
        rect1=bird->geometry();
        rect2=topLanqiu->geometry();
        rect3=bottomLanqiu->geometry();
        return rect1.intersects(rect2) || rect1.intersects(rect3);
    }
    void loadHighScores()//加载数据
    {
        QFile file(":/data/datas.txt");
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file);
            while (!in.atEnd())
            {
                int score = in.readLine().toInt();
                highScores.append(score);
            }
            file.close();
        }
        std::sort(highScores.begin(), highScores.end(), std::greater<int>());
    }
    void updateHighScores()//更新数据
    {
        highScores.append(scores);
        std::sort(highScores.begin(), highScores.end(), std::greater<int>());
        while (highScores.size() > 5) // 仅保留前五名
            {
                highScores.pop_back();
            }
            QFile file(":/data/datas.txt");
            if (file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream out(&file);
                for (int score : highScores)
                {
                    out << score << "\n";
                }
                file.close();
            }
    }
    void displayScoreBoard()//展示排行榜
    {
        QString scoresText = "排行榜\n";
        for (int i = 0; i < highScores.size(); ++i)
        {
            scoresText += QString::number(i + 1) + ". " + QString::number(highScores.at(i)) + "\n";
        }
        scoreBoard->setText(scoresText);
        showScoreBtn->hide();
        scoreBoard->show();
        closeScoreBoard->show();
    }
    void CloseScore()
    {
        showScoreBtn->show();
        closeScoreBoard->hide();
        scoreBoard->hide();
    }

protected:
    //重载鼠标事件
    void mousePressEvent(QMouseEvent *event)
    {   //点击鼠标使小鸟上升
        birdVelocity=0;
        Q_UNUSED(event);
        moveBirdUp();
    }

private:
    GameState gameState;
    QPushButton *btn1;
    QPushButton *btn2;
    QLabel *bird;
    QLabel *background;
    QLabel *gameout;
    QTimer *timer;
    int birdVelocity;//速度
    int gravity;//重力加速度
    QLabel *topLanqiu;
    QLabel *bottomLanqiu;
    QRandomGenerator random;
    QRect rect1;
    QRect rect2;
    QRect rect3;
    int scores=0;
    QLabel *scoreLabel;
    QLabel *count;
    QLabel *tips;
    QMediaPlayer Welcome;//欢迎音乐
    QMediaPlayer Clicked;//点击音乐
    QMediaPlayer Bgm;//背景音乐
    QMediaPlayer failure;//失败音乐
    QMediaPlayer stage_award;//阶段性反馈音乐

    QPushButton *btncommon;
    QPushButton *btnhell;//地狱模式
    QLabel *Choose;
    int sudu;
    QLabel *hello;

    QVector<int> highScores; // 存储排行榜数据
    QTextEdit *scoreBoard; // 用于显示排行榜
    QPushButton *showScoreBtn; // 用于显示排行榜按钮
    QPushButton *closeScoreBoard;//关闭排行榜

    QPushButton *closeGame;
};
int main(int argc, char *argv[])
{
    QApplication app(argc,argv);
    Mainwindow window;
    window.show();

    return app.exec();
}
