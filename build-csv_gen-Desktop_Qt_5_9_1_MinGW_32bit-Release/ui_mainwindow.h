/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QLabel *filename_l;
    QPushButton *start_pb;
    QPushButton *stop_pb;
    QPushButton *filepicker_pb;
    QLabel *com_l;
    QComboBox *com_cb;
    QPushButton *refresh_pb;
    QLineEdit *filename_le;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QMenuBar *menuBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(230, 150);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        filename_l = new QLabel(centralWidget);
        filename_l->setObjectName(QStringLiteral("filename_l"));
        filename_l->setGeometry(QRect(10, 40, 51, 16));
        start_pb = new QPushButton(centralWidget);
        start_pb->setObjectName(QStringLiteral("start_pb"));
        start_pb->setGeometry(QRect(10, 70, 75, 23));
        stop_pb = new QPushButton(centralWidget);
        stop_pb->setObjectName(QStringLiteral("stop_pb"));
        stop_pb->setEnabled(false);
        stop_pb->setGeometry(QRect(90, 70, 75, 23));
        stop_pb->setAutoDefault(false);
        stop_pb->setFlat(false);
        filepicker_pb = new QPushButton(centralWidget);
        filepicker_pb->setObjectName(QStringLiteral("filepicker_pb"));
        filepicker_pb->setGeometry(QRect(200, 40, 21, 23));
        com_l = new QLabel(centralWidget);
        com_l->setObjectName(QStringLiteral("com_l"));
        com_l->setGeometry(QRect(10, 10, 51, 16));
        com_cb = new QComboBox(centralWidget);
        com_cb->setObjectName(QStringLiteral("com_cb"));
        com_cb->setGeometry(QRect(70, 10, 69, 22));
        refresh_pb = new QPushButton(centralWidget);
        refresh_pb->setObjectName(QStringLiteral("refresh_pb"));
        refresh_pb->setGeometry(QRect(150, 10, 75, 23));
        filename_le = new QLineEdit(centralWidget);
        filename_le->setObjectName(QStringLiteral("filename_le"));
        filename_le->setGeometry(QRect(70, 40, 113, 20));
        filename_le->setReadOnly(true);
        MainWindow->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 230, 21));
        MainWindow->setMenuBar(menuBar);

        retranslateUi(MainWindow);

        stop_pb->setDefault(false);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        MainWindow->setToolTip(QApplication::translate("MainWindow", "Start acquistion.", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        filename_l->setText(QApplication::translate("MainWindow", "Filename:", Q_NULLPTR));
        start_pb->setText(QApplication::translate("MainWindow", "Start", Q_NULLPTR));
        stop_pb->setText(QApplication::translate("MainWindow", "Stop", Q_NULLPTR));
        filepicker_pb->setText(QApplication::translate("MainWindow", "...", Q_NULLPTR));
        com_l->setText(QApplication::translate("MainWindow", "COM port:", Q_NULLPTR));
        refresh_pb->setText(QApplication::translate("MainWindow", "Refresh", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
