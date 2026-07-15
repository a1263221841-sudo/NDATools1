/********************************************************************************
** Form generated from reading UI file 'formdataprocessor.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FORMDATAPROCESSOR_H
#define UI_FORMDATAPROCESSOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>
#include "formchilddataprocessor.h"
#include "formchilddatavalidation.h"

QT_BEGIN_NAMESPACE

class Ui_FormdataProcessor
{
public:
    QTabWidget *tabWidget_dataConversion;
    FormChildDataProcessor *tab_DataConversion;
    FormchildDataValidation *tab_DataValidation;
    QWidget *tab_5;

    void setupUi(QWidget *FormdataProcessor)
    {
        if (FormdataProcessor->objectName().isEmpty())
            FormdataProcessor->setObjectName("FormdataProcessor");
        FormdataProcessor->resize(860, 560);
        tabWidget_dataConversion = new QTabWidget(FormdataProcessor);
        tabWidget_dataConversion->setObjectName("tabWidget_dataConversion");
        tabWidget_dataConversion->setGeometry(QRect(10, 10, 841, 541));
        tab_DataConversion = new FormChildDataProcessor();
        tab_DataConversion->setObjectName("tab_DataConversion");
        tabWidget_dataConversion->addTab(tab_DataConversion, QString());
        tab_DataValidation = new FormchildDataValidation();
        tab_DataValidation->setObjectName("tab_DataValidation");
        tabWidget_dataConversion->addTab(tab_DataValidation, QString());
        tab_5 = new QWidget();
        tab_5->setObjectName("tab_5");
        tabWidget_dataConversion->addTab(tab_5, QString());

        retranslateUi(FormdataProcessor);

        tabWidget_dataConversion->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(FormdataProcessor);
    } // setupUi

    void retranslateUi(QWidget *FormdataProcessor)
    {
        FormdataProcessor->setWindowTitle(QCoreApplication::translate("FormdataProcessor", "Form", nullptr));
        tabWidget_dataConversion->setTabText(tabWidget_dataConversion->indexOf(tab_DataConversion), QCoreApplication::translate("FormdataProcessor", "\346\225\260\346\215\256\350\275\254\346\215\242", nullptr));
        tabWidget_dataConversion->setTabText(tabWidget_dataConversion->indexOf(tab_DataValidation), QCoreApplication::translate("FormdataProcessor", "\346\225\260\346\215\256\346\240\241\351\252\214", nullptr));
        tabWidget_dataConversion->setTabText(tabWidget_dataConversion->indexOf(tab_5), QCoreApplication::translate("FormdataProcessor", "\351\241\265", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FormdataProcessor: public Ui_FormdataProcessor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORMDATAPROCESSOR_H
