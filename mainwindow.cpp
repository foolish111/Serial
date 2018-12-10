#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->setWindowFlags(Qt::Dialog);
    ui->setupUi(this);

    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        if (serial.open(QIODevice::ReadWrite))
        {
            ui->PortBox->addItem(serial.portName());
            serial.close();
        }
    }
    ui->BaudBox->setCurrentIndex(1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_quitButton_clicked()
{
    qApp->exit(777);
}

/*
 *
 * 打开串口槽函数
 *
 */
void MainWindow::on_OpenSerialButton_clicked()
{
    if (ui->OpenSerialButton->text() == tr("打开串口"))
    {
        serial = new QSerialPort;
        serial->setPortName(ui->PortBox->currentText());
        serial->open(QIODevice::ReadWrite);
        if (ui->BaudBox->currentIndex() == 0)
            serial->setBaudRate(QSerialPort::Baud1200);
        else if (ui->BaudBox->currentIndex() == 1)
            serial->setBaudRate(QSerialPort::Baud2400);
        else if (ui->BaudBox->currentIndex() == 2)
            serial->setBaudRate(QSerialPort::Baud4800);
        else if (ui->BaudBox->currentIndex() == 3)
            serial->setBaudRate(QSerialPort::Baud9600);
        else if (ui->BaudBox->currentIndex() == 4)
            serial->setBaudRate(QSerialPort::Baud19200);
        else if (ui->BaudBox->currentIndex() == 5)
            serial->setBaudRate(QSerialPort::Baud38400);
        else if (ui->BaudBox->currentIndex() == 6)
            serial->setBaudRate(QSerialPort::Baud57600);
        else if (ui->BaudBox->currentIndex() == 7)
            serial->setBaudRate(QSerialPort::Baud115200);
        switch (ui->BitBox->currentIndex())
        {
        case 8:
            serial->setDataBits(QSerialPort::Data8);
            break;
        default:
            break;
        }
        switch (ui->ParityBox->currentIndex())
        {
        case 0:
            serial->setParity(QSerialPort::NoParity);
            break;
        default:
            break;
        }
        switch (ui->BitBox->currentIndex())
        {
        case 1:
            serial->setStopBits(QSerialPort::OneStop);
            break;
        case 2:
            serial->setStopBits(QSerialPort::TwoStop);
            break;
        default:
            break;
        }
        serial->setFlowControl(QSerialPort::NoFlowControl);

        ui->PortBox->setEnabled(false);
        ui->BaudBox->setEnabled(false);
        ui->BitBox->setEnabled(false);
        ui->StopBit->setEnabled(false);
        ui->ParityBox->setEnabled(false);
        ui->OpenSerialButton->setText(tr("关闭串口"));

        QObject::connect(serial, &QSerialPort::readyRead, this, &MainWindow::ReadData);
    }
    else
    {
        serial->clear();
        serial->close();
        serial->deleteLater();

        ui->PortBox->setEnabled(true);
        ui->BaudBox->setEnabled(true);
        ui->BitBox->setEnabled(true);
        ui->StopBit->setEnabled(true);
        ui->ParityBox->setEnabled(true);
        ui->OpenSerialButton->setText(tr("打开串口"));

    }
}

/*
 *
 * 串口读数据槽函数
 *
 */
void MainWindow::ReadData()
{
    int bytelen = serial->bytesAvailable();
    if (bytelen <= 0)
        return;
    qDebug("byteLen=%d\n", bytelen);
    QByteArray buf = serial->readAll();
    if (this->ui->checkBox_2->isChecked())
    {
        QString   temp = buf.toHex().data();
        QString temp1;
        for (int i=0; i<temp.length()/2; i++)
        {
            char s = temp.toLatin1().at(i);
            if ( s >= 'a' && s <= 'f')
            {
                s -= 32;
                temp1 += s+temp.mid(i*2+1, 1)+" ";
            }
            else
                temp1 += temp.mid(i*2,2)+" ";
        }
        ui->textEdit->append(temp1);
    }
    else
        ui->textEdit->append(buf);
}

/*
 *
 * 清空接收区
 *
 */
void MainWindow::on_clearButton_clicked()
{
    ui->textEdit->clear();
}

/*
 *
 * 清空输入区
 *
 */
void MainWindow::on_clearInputButton_clicked()
{
    ui->textEdit_2->clear();
}

/*
 *
 * 发送按钮槽函数
 *
 */
void MainWindow::on_SendButton_clicked()
{
    QString str = ui->textEdit_2->toPlainText();
    qDebug(str.toLatin1());
    QByteArray bytearray;
    if (ui->checkBox->isChecked() == true)
    {
       bytearray = QString2Hex(str);
       serial->write(bytearray);
    }
    else
        serial->write(str.toLatin1());
    qDebug(str.toLatin1());
}

/*
 *
 * 将字符串转为16进制
 *
 */
QByteArray MainWindow::QString2Hex(QString str)
{
    QByteArray senddata;
    qDebug("QString2Hex\n");

    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len/2);
    char lstr, hstr;
    for (int i=0; i<len;)
    {
        hstr = str[i].toLatin1();
        if (hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if (i >= len)
            break;
        lstr = str[i].toLatin1();
        if (hstr >= 'a' && hstr <= 'f')
            hstr -= 32;
        if (lstr >= 'a' && lstr <= 'f')
            lstr -= 32;
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if ((hexdata == 16) || lowhexdata == 16)
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
    return senddata;
}

/*
 *
 * 将单个字符转换成16进制
 *
 */
char MainWindow::ConvertHexChar(char ch)
{
    qDebug("ConvertHexChar\n");
    if ((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if ((ch >= 'A') && ch <= 'F')
        return ch-'A'+10;
    else if ((ch > 'a') && ch <= 'f')
        return ch-'a'+10;
    else
        return -1;
}
