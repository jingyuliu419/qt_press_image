/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QWidget *plotWidget1;
    QTextEdit *img_label_3;
    QTextEdit *classtextEdit;
    QPushButton *pushButton2;
    QPushButton *pushButton1;
    QLabel *txt_label1;
    QLineEdit *lineEdit1;
    QLabel *txt_label;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1600, 1600);
        MainWindow->setMinimumSize(QSize(1600, 1600));
        MainWindow->setMaximumSize(QSize(1600, 1600));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        plotWidget1 = new QWidget(centralwidget);
        plotWidget1->setObjectName(QString::fromUtf8("plotWidget1"));
        plotWidget1->setGeometry(QRect(131, 800, 1105, 500));
        plotWidget1->setMinimumSize(QSize(1026, 500));
        plotWidget1->setMaximumSize(QSize(1126, 16777215));
        img_label_3 = new QTextEdit(centralwidget);
        img_label_3->setObjectName(QString::fromUtf8("img_label_3"));
        img_label_3->setGeometry(QRect(130, 60, 721, 721));
        classtextEdit = new QTextEdit(centralwidget);
        classtextEdit->setObjectName(QString::fromUtf8("classtextEdit"));
        classtextEdit->setGeometry(QRect(880, 370, 361, 371));
        pushButton2 = new QPushButton(centralwidget);
        pushButton2->setObjectName(QString::fromUtf8("pushButton2"));
        pushButton2->setGeometry(QRect(880, 230, 131, 51));
        pushButton1 = new QPushButton(centralwidget);
        pushButton1->setObjectName(QString::fromUtf8("pushButton1"));
        pushButton1->setGeometry(QRect(880, 300, 131, 51));
        pushButton1->setMaximumSize(QSize(150, 16777215));
        txt_label1 = new QLabel(centralwidget);
        txt_label1->setObjectName(QString::fromUtf8("txt_label1"));
        txt_label1->setGeometry(QRect(1030, 230, 201, 51));
        lineEdit1 = new QLineEdit(centralwidget);
        lineEdit1->setObjectName(QString::fromUtf8("lineEdit1"));
        lineEdit1->setGeometry(QRect(1150, 230, 211, 41));
        txt_label = new QLabel(centralwidget);
        txt_label->setObjectName(QString::fromUtf8("txt_label"));
        txt_label->setGeometry(QRect(1030, 310, 331, 31));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1600, 28));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        pushButton2->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213", nullptr));
        pushButton1->setText(QCoreApplication::translate("MainWindow", "\344\272\213\345\220\216\345\210\206\346\236\220", nullptr));
        txt_label1->setText(QCoreApplication::translate("MainWindow", "\350\257\267\350\276\223\345\205\245\345\247\223\345\220\215\347\274\251\345\206\231\357\274\232", nullptr));
        lineEdit1->setText(QString());
        txt_label->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
