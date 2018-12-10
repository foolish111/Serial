#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private slots:
    void on_OpenSerialButton_clicked();
    void ReadData();
    void on_SendButton_clicked();
    void on_clearButton_clicked();
    void on_clearInputButton_clicked();
    void on_quitButton_clicked();

private:
    QSerialPort *serial;

    QByteArray QString2Hex(QString str);

    char ConvertHexChar(char ch);
};

#endif // MAINWINDOW_H
