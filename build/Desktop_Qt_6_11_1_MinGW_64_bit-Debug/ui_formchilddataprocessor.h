/********************************************************************************
** Form generated from reading UI file 'formchilddataprocessor.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FORMCHILDDATAPROCESSOR_H
#define UI_FORMCHILDDATAPROCESSOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FormChildDataProcessor
{
public:
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QTextEdit *textEdit_EnterData;
    QTextEdit *textEdit_BringData;
    QComboBox *comboBox_ConversionType;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_CoversionOpration;
    QPushButton *pushButton_DeclearData;
    QPushButton *pushButton_Change;
    QPushButton *pushButton_CoversionCopyResult;

    void setupUi(QWidget *FormChildDataProcessor)
    {
        if (FormChildDataProcessor->objectName().isEmpty())
            FormChildDataProcessor->setObjectName("FormChildDataProcessor");
        FormChildDataProcessor->resize(850, 560);
        label = new QLabel(FormChildDataProcessor);
        label->setObjectName("label");
        label->setGeometry(QRect(50, 20, 81, 21));
        QFont font;
        font.setFamilies({QString::fromUtf8("\345\256\213\344\275\223")});
        font.setPointSize(10);
        label->setFont(font);
        label_2 = new QLabel(FormChildDataProcessor);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(50, 60, 81, 21));
        label_2->setFont(font);
        label_3 = new QLabel(FormChildDataProcessor);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(540, 90, 81, 21));
        label_3->setFont(font);
        label_4 = new QLabel(FormChildDataProcessor);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(560, 60, 161, 21));
        label_4->setFont(font);
        label_5 = new QLabel(FormChildDataProcessor);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(50, 270, 81, 21));
        label_5->setFont(font);
        label_6 = new QLabel(FormChildDataProcessor);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(560, 260, 161, 21));
        label_6->setFont(font);
        textEdit_EnterData = new QTextEdit(FormChildDataProcessor);
        textEdit_EnterData->setObjectName("textEdit_EnterData");
        textEdit_EnterData->setGeometry(QRect(50, 100, 691, 151));
        textEdit_EnterData->setFont(font);
        textEdit_BringData = new QTextEdit(FormChildDataProcessor);
        textEdit_BringData->setObjectName("textEdit_BringData");
        textEdit_BringData->setGeometry(QRect(50, 300, 691, 151));
        textEdit_BringData->setFont(font);
        comboBox_ConversionType = new QComboBox(FormChildDataProcessor);
        comboBox_ConversionType->setObjectName("comboBox_ConversionType");
        comboBox_ConversionType->setGeometry(QRect(140, 20, 171, 25));
        comboBox_ConversionType->setFont(font);
        horizontalLayoutWidget = new QWidget(FormChildDataProcessor);
        horizontalLayoutWidget->setObjectName("horizontalLayoutWidget");
        horizontalLayoutWidget->setGeometry(QRect(40, 450, 731, 71));
        horizontalLayoutWidget->setFont(font);
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        pushButton_CoversionOpration = new QPushButton(horizontalLayoutWidget);
        pushButton_CoversionOpration->setObjectName("pushButton_CoversionOpration");
        pushButton_CoversionOpration->setFont(font);

        horizontalLayout->addWidget(pushButton_CoversionOpration);

        pushButton_DeclearData = new QPushButton(horizontalLayoutWidget);
        pushButton_DeclearData->setObjectName("pushButton_DeclearData");
        pushButton_DeclearData->setFont(font);

        horizontalLayout->addWidget(pushButton_DeclearData);

        pushButton_Change = new QPushButton(horizontalLayoutWidget);
        pushButton_Change->setObjectName("pushButton_Change");
        pushButton_Change->setFont(font);

        horizontalLayout->addWidget(pushButton_Change);

        pushButton_CoversionCopyResult = new QPushButton(horizontalLayoutWidget);
        pushButton_CoversionCopyResult->setObjectName("pushButton_CoversionCopyResult");
        pushButton_CoversionCopyResult->setFont(font);

        horizontalLayout->addWidget(pushButton_CoversionCopyResult);


        retranslateUi(FormChildDataProcessor);

        QMetaObject::connectSlotsByName(FormChildDataProcessor);
    } // setupUi

    void retranslateUi(QWidget *FormChildDataProcessor)
    {
        FormChildDataProcessor->setWindowTitle(QCoreApplication::translate("FormChildDataProcessor", "Form", nullptr));
        label->setText(QCoreApplication::translate("FormChildDataProcessor", "\350\275\254\346\215\242\347\261\273\345\236\213:", nullptr));
        label_2->setText(QCoreApplication::translate("FormChildDataProcessor", "\350\276\223\345\205\245\346\225\260\346\215\256:", nullptr));
        label_3->setText(QString());
        label_4->setText(QCoreApplication::translate("FormChildDataProcessor", "\345\255\227\347\254\246\346\225\260:  ,\345\255\227\350\212\202\346\225\260:", nullptr));
        label_5->setText(QCoreApplication::translate("FormChildDataProcessor", "\350\276\223\345\207\272\346\225\260\346\215\256:", nullptr));
        label_6->setText(QCoreApplication::translate("FormChildDataProcessor", "\345\255\227\347\254\246\346\225\260:  ,\345\255\227\350\212\202\346\225\260:", nullptr));
        pushButton_CoversionOpration->setText(QCoreApplication::translate("FormChildDataProcessor", "\350\275\254\346\215\242\346\223\215\344\275\234", nullptr));
        pushButton_DeclearData->setText(QCoreApplication::translate("FormChildDataProcessor", "\346\270\205\347\251\272\346\225\260\346\215\256", nullptr));
        pushButton_Change->setText(QCoreApplication::translate("FormChildDataProcessor", "\344\272\244\346\215\242\350\276\223\345\205\245\350\276\223\345\207\272", nullptr));
        pushButton_CoversionCopyResult->setText(QCoreApplication::translate("FormChildDataProcessor", "\345\244\215\345\210\266\347\273\223\346\236\234", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FormChildDataProcessor: public Ui_FormChildDataProcessor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORMCHILDDATAPROCESSOR_H
