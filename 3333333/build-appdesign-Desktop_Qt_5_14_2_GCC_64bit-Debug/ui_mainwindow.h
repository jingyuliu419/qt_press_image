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
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton1;
    QLabel *txt_label;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout;
    QTextEdit *img_label_2;
    QTextEdit *classtextEdit;
    QTextEdit *img_label_3;
    QWidget *plotWidget1;
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
        layoutWidget = new QWidget(centralwidget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(130, 22, 1107, 1279));
        verticalLayout_2 = new QVBoxLayout(layoutWidget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pushButton1 = new QPushButton(layoutWidget);
        pushButton1->setObjectName(QString::fromUtf8("pushButton1"));
        pushButton1->setMaximumSize(QSize(150, 16777215));

        horizontalLayout->addWidget(pushButton1);

        txt_label = new QLabel(layoutWidget);
        txt_label->setObjectName(QString::fromUtf8("txt_label"));

        horizontalLayout->addWidget(txt_label);


        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        img_label_2 = new QTextEdit(layoutWidget);
        img_label_2->setObjectName(QString::fromUtf8("img_label_2"));
        img_label_2->setMinimumSize(QSize(365, 365));
        img_label_2->setMaximumSize(QSize(365, 365));

        verticalLayout->addWidget(img_label_2);

        classtextEdit = new QTextEdit(layoutWidget);
        classtextEdit->setObjectName(QString::fromUtf8("classtextEdit"));

        verticalLayout->addWidget(classtextEdit);


        horizontalLayout_2->addLayout(verticalLayout);

        img_label_3 = new QTextEdit(layoutWidget);
        img_label_3->setObjectName(QString::fromUtf8("img_label_3"));
        img_label_3->setMinimumSize(QSize(730, 730));
        img_label_3->setMaximumSize(QSize(731, 730));

        horizontalLayout_2->addWidget(img_label_3);


        verticalLayout_2->addLayout(horizontalLayout_2);

        plotWidget1 = new QWidget(layoutWidget);
        plotWidget1->setObjectName(QString::fromUtf8("plotWidget1"));
        plotWidget1->setMinimumSize(QSize(1026, 500));
        plotWidget1->setMaximumSize(QSize(1126, 16777215));

        verticalLayout_2->addWidget(plotWidget1);

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
        pushButton1->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213\347\233\221\345\220\254", nullptr));
        txt_label->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
