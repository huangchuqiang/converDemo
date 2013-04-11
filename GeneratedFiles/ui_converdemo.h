/********************************************************************************
** Form generated from reading UI file 'converdemo.ui'
**
** Created: Thu Apr 11 16:27:38 2013
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONVERDEMO_H
#define UI_CONVERDEMO_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_converDemoClass
{
public:
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *converDemoClass)
    {
        if (converDemoClass->objectName().isEmpty())
            converDemoClass->setObjectName(QString::fromUtf8("converDemoClass"));
        converDemoClass->resize(1066, 733);
        centralWidget = new QWidget(converDemoClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        converDemoClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(converDemoClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1066, 23));
        converDemoClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(converDemoClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        converDemoClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(converDemoClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        converDemoClass->setStatusBar(statusBar);

        retranslateUi(converDemoClass);

        QMetaObject::connectSlotsByName(converDemoClass);
    } // setupUi

    void retranslateUi(QMainWindow *converDemoClass)
    {
        converDemoClass->setWindowTitle(QApplication::translate("converDemoClass", "converDemo", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class converDemoClass: public Ui_converDemoClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONVERDEMO_H
