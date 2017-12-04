#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QtSerialPort>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QString filename;
    QFile* file;
    QSerialPort* serial;

    void showStatusMessage(const QString &);

private slots:
    void refresh();
    void filepicker();
    void start(const char*);
    void stop();
    void clear();
    void readData();
};

#endif // MAINWINDOW_H
