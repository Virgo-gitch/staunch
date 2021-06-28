/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QGridLayout *gridLayout;
    QPushButton *pushButton_5;
    QLineEdit *lineEdit;
    QPushButton *pushButton_3;
    QPushButton *pushButton;
    QLineEdit *lineEdit_2;
    QPushButton *pushButton_4;
    QPushButton *pushButton_2;
    QTextEdit *textEdit;
    QLineEdit *outPutDirLineEdit;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(553, 496);
        gridLayout = new QGridLayout(Widget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        pushButton_5 = new QPushButton(Widget);
        pushButton_5->setObjectName(QString::fromUtf8("pushButton_5"));
        pushButton_5->setMinimumSize(QSize(80, 40));
        pushButton_5->setMaximumSize(QSize(40, 40));

        gridLayout->addWidget(pushButton_5, 0, 3, 1, 1);

        lineEdit = new QLineEdit(Widget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setMinimumSize(QSize(0, 30));
        QFont font;
        font.setFamily(QString::fromUtf8("Arial"));
        font.setPointSize(12);
        lineEdit->setFont(font);

        gridLayout->addWidget(lineEdit, 1, 0, 1, 1);

        pushButton_3 = new QPushButton(Widget);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        pushButton_3->setMinimumSize(QSize(40, 40));
        pushButton_3->setMaximumSize(QSize(40, 40));

        gridLayout->addWidget(pushButton_3, 1, 1, 1, 1);

        pushButton = new QPushButton(Widget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setMinimumSize(QSize(80, 40));
        pushButton->setMaximumSize(QSize(80, 40));

        gridLayout->addWidget(pushButton, 1, 2, 1, 2);

        lineEdit_2 = new QLineEdit(Widget);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));
        lineEdit_2->setMinimumSize(QSize(0, 30));
        lineEdit_2->setFont(font);

        gridLayout->addWidget(lineEdit_2, 2, 0, 1, 1);

        pushButton_4 = new QPushButton(Widget);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
        pushButton_4->setMinimumSize(QSize(40, 40));
        pushButton_4->setMaximumSize(QSize(40, 40));

        gridLayout->addWidget(pushButton_4, 2, 1, 1, 1);

        pushButton_2 = new QPushButton(Widget);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setMinimumSize(QSize(80, 40));
        pushButton_2->setMaximumSize(QSize(80, 40));

        gridLayout->addWidget(pushButton_2, 2, 2, 1, 2);

        textEdit = new QTextEdit(Widget);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));

        gridLayout->addWidget(textEdit, 3, 0, 1, 4);

        outPutDirLineEdit = new QLineEdit(Widget);
        outPutDirLineEdit->setObjectName(QString::fromUtf8("outPutDirLineEdit"));
        outPutDirLineEdit->setMinimumSize(QSize(0, 30));
        outPutDirLineEdit->setFont(font);

        gridLayout->addWidget(outPutDirLineEdit, 0, 0, 1, 2);


        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        pushButton_5->setText(QCoreApplication::translate("Widget", "\350\276\223\345\207\272\346\240\271\347\233\256\345\275\225", nullptr));
        lineEdit->setPlaceholderText(QCoreApplication::translate("Widget", "Zip file", nullptr));
        pushButton_3->setText(QCoreApplication::translate("Widget", "\346\226\207\344\273\266", nullptr));
        pushButton->setText(QCoreApplication::translate("Widget", "\350\247\243\345\216\213\346\226\207\344\273\266", nullptr));
        lineEdit_2->setPlaceholderText(QCoreApplication::translate("Widget", "Zip file directory", nullptr));
        pushButton_4->setText(QCoreApplication::translate("Widget", "\347\233\256\345\275\225", nullptr));
        pushButton_2->setText(QCoreApplication::translate("Widget", "\346\211\271\351\207\217\350\247\243\345\216\213", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
