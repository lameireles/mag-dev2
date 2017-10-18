#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    file = new QFile(filename);
    serial = new QSerialPort(this);
    serial->setBaudRate(115200);

    connect(ui->refresh_pb, SIGNAL(released()), this, SLOT(refresh()));
    connect(ui->filepicker_pb, SIGNAL (released()), this, SLOT(filepicker()));
    connect(ui->start_pb, SIGNAL(released()), this, SLOT(start()));
    connect(ui->stop_pb, SIGNAL(released()), this, SLOT(stop()));
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete file;
    delete serial;
}

void MainWindow::showStatusMessage(const QString & str)
{
    ui->statusBar->showMessage(str);
}

void MainWindow::refresh()
{
    ui->com_cb->clear();
    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
    for (int i = 0; i < list.length(); i++) {
        ui->com_cb->addItem(list.at(i).portName());
    }
}

void MainWindow::filepicker()
{
    filename = QFileDialog::getSaveFileName(this,
        tr("Filename"), "",
        tr("CSV file (*.csv)"));
    if (!filename.isEmpty()) ui->filename_le->setText(filename);
}

void MainWindow::start()
{
    file->setFileName(filename);
    if (!file->open(QIODevice::WriteOnly|QIODevice::Truncate)) {
        QMessageBox::critical(this, tr("Error"), file->errorString());
        showStatusMessage(tr("File open error"));
        return;
    }

    serial->setPortName(ui->com_cb->currentText());
    if (!serial->open(QIODevice::ReadWrite)) {
        QMessageBox::critical(this, tr("Error"), serial->errorString());
        showStatusMessage(tr("Serial open error"));
        if (file->isOpen()) file->close();
        return;
    }

    showStatusMessage(tr("Connected to %1").arg(serial->portName()));
    ui->start_pb->setEnabled(false);
    ui->stop_pb->setEnabled(true);
    serial->write("a");
}

void MainWindow::stop()
{
    serial->write("b");
    serial->flush();
    if (serial->isOpen()) serial->close();
    if (file->isOpen()) file->close();
    showStatusMessage(tr("Disconnected"));
    ui->stop_pb->setEnabled(false);
    ui->start_pb->setEnabled(true);
}

void MainWindow::readData()
{
    if (file->isOpen()) {
        file->write(serial->readAll());
        file->flush();
    }
}
