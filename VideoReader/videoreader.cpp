#include "videoreader.h"
#include "ui_videoreader.h"

QString format = "mm:ss";
bool    press = false;


VideoReader::VideoReader(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::VideoReader)
    , filename(NULL)
{
    QScreen *screen = QGuiApplication::primaryScreen();

    QRect   dim = screen->availableGeometry();

    ui->setupUi(this);
    /*centrage de la fenetre*/
    int x = (dim.width() - width()) / 2;
    int y = (dim.height() - height()) / 2;
    setGeometry(x, y, width(), height());
    titleBar = ui->header;
    this->setWindowFlag(Qt::FramelessWindowHint);
    vPlayer = new QMediaPlayer;
    vOutput = new QVideoWidget;
    QHBoxLayout *videoLayout = new QHBoxLayout(ui->groupBox);
    videoLayout->setContentsMargins(0, 0, 0, 0);
    videoLayout->addWidget(vOutput);
    vOutput->setVisible(true);
    vOutput->setMinimumSize(ui->groupBox->minimumSize());
    if (vPlayer){
        audioOut = new QAudioOutput;
        if (audioOut){
            vPlayer->setAudioOutput(audioOut);
            audioOut->setVolume(0.5);
        }
        if (vOutput)
            vPlayer->setVideoOutput(vOutput);
    }
    connect(vPlayer, &QMediaPlayer::durationChanged, this, &VideoReader::detectTimeVariation);
    connect(vPlayer, &QMediaPlayer::positionChanged, this, &VideoReader::positionChanged);
    connect(ui->reduce, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(ui->expand, &QPushButton::clicked, [this]() {
        if (isMaximized()) {
            showNormal();
        } else {
            showMaximized();
        }
    });
    ui->videoProgress->setRange(0, vPlayer->duration() / 1000);
    ui->volumeAjust->setMinimum(1);
    ui->volumeAjust->setMaximum(100);
    ui->volumeAjust->setValue(50);
}

void VideoReader::detectTimeVariation(qint64 duration) {
    Mduration = duration / 1000;
    ui->videoProgress->setMaximum(Mduration);
}

void    VideoReader::updateTime(qint64 newValue){
    QTime   timeTotal((Mduration / 3600) % 60, (Mduration / 60) % 60, Mduration % 60, (Mduration * 1000) % 1000);
    if (Mduration > 3600)
        format = "hh:mm:ss";
    ui->totalTime->setText(timeTotal.toString(format));
    QTime current((newValue / 3600) % 60, (newValue / 60) % 60, newValue % 60, (newValue * 1000) % 1000);
    ui->currTime->setText(current.toString(format));
}

void VideoReader::positionChanged(qint64 progress){
    if (!ui->videoProgress->isSliderDown()){
        ui->videoProgress->setValue(progress / 1000);
    }
    updateTime(progress / 1000);
}


VideoReader::~VideoReader()
{
    delete ui;
    if (filename){
        delete filename;
        filename = NULL;
    }
    if (audioOut){
        delete audioOut;
    }
    if (vOutput){
        delete vOutput;
    }
    if (vPlayer){
        delete vPlayer;
    }
}


void VideoReader::on_close_clicked()
{
    QApplication::quit();
}


void VideoReader::on_option_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, "Open Video", "/home", "*.mp4 *.MP4 *.avi *.mkv *.ts");
    if (!file.isEmpty()){
        if (filename){
            delete filename;
            filename = NULL;
        }
        filename = new QString(file);
        vPlayer->setPosition(0);
        vPlayer->setSource(QUrl::fromLocalFile(*filename));
        vOutput->show();
    }
}


void VideoReader::on_Play_clicked()
{
    vPlayer->play();
}


void VideoReader::on_pause_clicked()
{
    vPlayer->pause();
}


void VideoReader::on_stop_clicked()
{
    vPlayer->stop();
    vPlayer->setPosition(0);
}



void VideoReader::on_volumeAjust_valueChanged(int value)
{
    if (audioOut){
        float newVol = (float)value / 100;
        audioOut->setVolume(newVol);
    }
}


void VideoReader::on_videoProgress_valueChanged(int value)
{
    if (vPlayer && press == true){
        if (value == 0 || value * 1000 > (Mduration * 0.95) * 1000)
            return ;
        vPlayer->setPosition(value * 1000);
    }
}


void VideoReader::on_videoProgress_sliderPressed()
{
    press = true;
    if (vPlayer)
        vPlayer->pause();
}


void VideoReader::on_videoProgress_sliderReleased()
{
    press = false;
    if (vPlayer)
        vPlayer->play();
}

