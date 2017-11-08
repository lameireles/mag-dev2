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

    connect(ui->refresh_pb, &QPushButton::released, this, &MainWindow::refresh);
    connect(ui->filepicker_pb, &QPushButton::released, this, &MainWindow::filepicker);
    connect(ui->startA_pb, &QPushButton::released, this, [this]{start("a");});
    connect(ui->startB_pb, &QPushButton::released, this, [this]{start("b");});
    connect(ui->stop_pb, &QPushButton::released, this, &MainWindow::stop);
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
    ui->filename_le->setText(filename);
}

void MainWindow::start(const char* c)
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

    showStatusMessage(tr("Connected to %1 ('%2')").arg(serial->portName()).arg(c));
    ui->startA_pb->setEnabled(false);
    ui->startB_pb->setEnabled(false);
    ui->refresh_pb->setEnabled(false);
    ui->filepicker_pb->setEnabled(false);
    ui->com_cb->setEnabled(false);
    ui->stop_pb->setEnabled(true);
    serial->write(c);
}

void MainWindow::stop()
{
    serial->write("s");
    serial->flush();
    if (serial->isOpen()) serial->close();
    if (file->isOpen()) file->close();
    showStatusMessage(tr("Disconnected"));
    ui->stop_pb->setEnabled(false);
    ui->startA_pb->setEnabled(true);
    ui->startB_pb->setEnabled(true);
    ui->refresh_pb->setEnabled(true);
    ui->filepicker_pb->setEnabled(true);
    ui->com_cb->setEnabled(true);
}

void MainWindow::readData()
{
    if (file->isOpen()) {
        file->write(serial->readAll());
        file->flush();
    }
}
