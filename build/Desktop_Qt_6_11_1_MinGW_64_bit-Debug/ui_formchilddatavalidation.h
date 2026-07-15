/********************************************************************************
** Form generated from reading UI file 'formchilddatavalidation.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FORMCHILDDATAVALIDATION_H
#define UI_FORMCHILDDATAVALIDATION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FormchildDataValidation
{
public:
    QTextEdit *textEdit_Enter;
    QPushButton *pushButton_Calculate;
    QPushButton *pushButton_ValidationCopy;
    QCheckBox *checkBox_Unpper;
    QGroupBox *groupBox;
    QPlainTextEdit *plainTextEdit_CRC16;
    QPlainTextEdit *plainTextEdit_CRC32;
    QPlainTextEdit *plainTextEdit_MD5;
    QPlainTextEdit *plainTextEdit_SHA1;
    QPlainTextEdit *plainTextEdit_SHA256;
    QLabel *label;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QGroupBox *groupBox_2;

    void setupUi(QWidget *FormchildDataValidation)
    {
        if (FormchildDataValidation->objectName().isEmpty())
            FormchildDataValidation->setObjectName("FormchildDataValidation");
        FormchildDataValidation->resize(860, 560);
        textEdit_Enter = new QTextEdit(FormchildDataValidation);
        textEdit_Enter->setObjectName("textEdit_Enter");
        textEdit_Enter->setGeometry(QRect(43, 44, 781, 141));
        pushButton_Calculate = new QPushButton(FormchildDataValidation);
        pushButton_Calculate->setObjectName("pushButton_Calculate");
        pushButton_Calculate->setGeometry(QRect(50, 210, 93, 28));
        pushButton_ValidationCopy = new QPushButton(FormchildDataValidation);
        pushButton_ValidationCopy->setObjectName("pushButton_ValidationCopy");
        pushButton_ValidationCopy->setGeometry(QRect(170, 210, 131, 28));
        checkBox_Unpper = new QCheckBox(FormchildDataValidation);
        checkBox_Unpper->setObjectName("checkBox_Unpper");
        checkBox_Unpper->setGeometry(QRect(330, 210, 98, 23));
        groupBox = new QGroupBox(FormchildDataValidation);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(30, 250, 791, 251));
        plainTextEdit_CRC16 = new QPlainTextEdit(groupBox);
        plainTextEdit_CRC16->setObjectName("plainTextEdit_CRC16");
        plainTextEdit_CRC16->setGeometry(QRect(100, 20, 641, 31));
        plainTextEdit_CRC32 = new QPlainTextEdit(groupBox);
        plainTextEdit_CRC32->setObjectName("plainTextEdit_CRC32");
        plainTextEdit_CRC32->setGeometry(QRect(100, 60, 641, 31));
        plainTextEdit_MD5 = new QPlainTextEdit(groupBox);
        plainTextEdit_MD5->setObjectName("plainTextEdit_MD5");
        plainTextEdit_MD5->setGeometry(QRect(100, 100, 641, 31));
        plainTextEdit_SHA1 = new QPlainTextEdit(groupBox);
        plainTextEdit_SHA1->setObjectName("plainTextEdit_SHA1");
        plainTextEdit_SHA1->setGeometry(QRect(100, 150, 641, 31));
        plainTextEdit_SHA256 = new QPlainTextEdit(groupBox);
        plainTextEdit_SHA256->setObjectName("plainTextEdit_SHA256");
        plainTextEdit_SHA256->setGeometry(QRect(100, 200, 641, 31));
        label = new QLabel(groupBox);
        label->setObjectName("label");
        label->setGeometry(QRect(20, 30, 69, 19));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(20, 70, 69, 19));
        label_4 = new QLabel(groupBox);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(20, 110, 69, 19));
        label_5 = new QLabel(groupBox);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(20, 150, 69, 19));
        label_6 = new QLabel(groupBox);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(20, 200, 69, 19));
        groupBox_2 = new QGroupBox(FormchildDataValidation);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setGeometry(QRect(30, 20, 811, 181));
        groupBox_2->raise();
        textEdit_Enter->raise();
        pushButton_Calculate->raise();
        pushButton_ValidationCopy->raise();
        checkBox_Unpper->raise();
        groupBox->raise();

        retranslateUi(FormchildDataValidation);

        QMetaObject::connectSlotsByName(FormchildDataValidation);
    } // setupUi

    void retranslateUi(QWidget *FormchildDataValidation)
    {
        FormchildDataValidation->setWindowTitle(QCoreApplication::translate("FormchildDataValidation", "Form", nullptr));
        pushButton_Calculate->setText(QCoreApplication::translate("FormchildDataValidation", "\350\256\241\347\256\227\346\240\241\351\252\214\345\200\274", nullptr));
        pushButton_ValidationCopy->setText(QCoreApplication::translate("FormchildDataValidation", "\345\244\215\345\210\266\346\211\200\346\234\211\346\240\241\351\252\214\345\200\274", nullptr));
        checkBox_Unpper->setText(QCoreApplication::translate("FormchildDataValidation", "\345\244\247\345\206\231\346\230\276\347\244\272", nullptr));
        groupBox->setTitle(QCoreApplication::translate("FormchildDataValidation", "\346\240\241\351\252\214\345\200\274\347\273\223\346\236\234", nullptr));
        label->setText(QCoreApplication::translate("FormchildDataValidation", "CRC16:", nullptr));
        label_3->setText(QCoreApplication::translate("FormchildDataValidation", "CRC32:", nullptr));
        label_4->setText(QCoreApplication::translate("FormchildDataValidation", "MD5:", nullptr));
        label_5->setText(QCoreApplication::translate("FormchildDataValidation", "SHA1:", nullptr));
        label_6->setText(QCoreApplication::translate("FormchildDataValidation", "SHA256:", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("FormchildDataValidation", "\350\276\223\345\205\245\346\225\260\346\215\256", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FormchildDataValidation: public Ui_FormchildDataValidation {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORMCHILDDATAVALIDATION_H
