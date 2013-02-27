/********************************************************************************
** Form generated from reading UI file 'genphotoview.ui'
**
** Created: Thu May 19 01:11:20 2011
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GENPHOTOVIEW_H
#define UI_GENPHOTOVIEW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>
#include "genphotobrowser.h"

QT_BEGIN_NAMESPACE

class Ui_GenPhotoView
{
public:
    GenPhotoBrowser *photoBrowser;

    void setupUi(QWidget *GenPhotoView)
    {
        if (GenPhotoView->objectName().isEmpty())
            GenPhotoView->setObjectName(QString::fromUtf8("GenPhotoView"));
        GenPhotoView->resize(360, 490);
        photoBrowser = new GenPhotoBrowser(GenPhotoView);
        photoBrowser->setObjectName(QString::fromUtf8("photoBrowser"));
        photoBrowser->setGeometry(QRect(0, 0, 360, 490));

        retranslateUi(GenPhotoView);

        QMetaObject::connectSlotsByName(GenPhotoView);
    } // setupUi

    void retranslateUi(QWidget *GenPhotoView)
    {
        GenPhotoView->setWindowTitle(QApplication::translate("GenPhotoView", "Form", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class GenPhotoView: public Ui_GenPhotoView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GENPHOTOVIEW_H
