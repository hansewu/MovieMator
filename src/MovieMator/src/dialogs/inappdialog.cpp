/*
 * Copyright (c) 2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "inappdialog.h"
#include "ui_inappdialog.h"
#include "../widgets/twolinebutton.h"
#include <settings.h>
#include <QDesktopServices>
#include <Logger.h>

#define MONTHLY_SUBSCRIPTION "net.moviemator.moviemator.subscription_one_month"
#define THRERE_MONTH_SUBSCRIPTION "net.moviemator.moviemator.subscription_three_months"
#define YEARLY_SUBSCRIPTION "net.moviemator.moviemator.subscription_one_year"

static void inAppCallback(void *caller, int result)
{
    InAppDialog *dialog = (InAppDialog *)caller;

    //验证收据
    if (result == 0 && inapp_has_valid_subscription() >=0)
    {
        Settings.setIsSubscribed(true);
        dialog->done(0);
        LOG_DEBUG() << "purchase complete, verifiy success!";
    }
    else if (result == 0 && inapp_has_valid_subscription() < 0)
    {
        Settings.setIsSubscribed(true);
        dialog->done(0);
        LOG_DEBUG() << "purchase complete, verifiy failed!";
        inapp_refresh_receipt();
    }
    else
    {
        dialog->done(0);
        LOG_DEBUG() << "purchase failed!";
    }
}

InAppDialog::InAppDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InAppDialog)
{
    ui->setupUi(this);
    ui->useFreeVersionButton->setMainTitle(tr("Use Trial"));
    ui->useFreeVersionButton->setSubTitle(tr("Output videos of 5 minutes at most and will be saved with a watermark"));
    ui->useFreeVersionButton->setAccessoryTitle(tr("Free"));
    ui->paymentTipsTextEdit->setHidden(true);
    inapp_init(inAppCallback, (void*)this);
}

InAppDialog::~InAppDialog()
{
    delete ui;
}

void InAppDialog::setInAppProductInfo(INAPP_PRODUCT_INFO *pProductInfos, int nProductCount)
{
    for (int i = 0; i< nProductCount; i++)
    {
        INAPP_PRODUCT_INFO productInfo = pProductInfos[i];

        if(strcmp(productInfo.identifier, MONTHLY_SUBSCRIPTION) == 0)
        {
            //ui->monthlySubscriptionButton->setText(productInfo.name);
            ui->monthlySubscriptionButton->setMainTitle(productInfo.name);
            ui->monthlySubscriptionButton->setSubTitle(productInfo.description);
            ui->monthlySubscriptionButton->setAccessoryTitle(productInfo.price);
            ui->monthlySubscriptionButton->setObjectName(productInfo.identifier);
        }

        if(strcmp(productInfo.identifier, THRERE_MONTH_SUBSCRIPTION) == 0)
        {
            //ui->triMonthSubscriptionButton->setText(productInfo.name);
            ui->triMonthSubscriptionButton->setMainTitle(productInfo.name);
            ui->triMonthSubscriptionButton->setSubTitle(productInfo.description);
            ui->triMonthSubscriptionButton->setAccessoryTitle(productInfo.price);
            ui->triMonthSubscriptionButton->setObjectName(productInfo.identifier);
        }

        if(strcmp(productInfo.identifier, YEARLY_SUBSCRIPTION) == 0)
        {
            //ui->yearlySubscriptionButton->setText(productInfo.name);
            ui->yearlySubscriptionButton->setMainTitle(productInfo.name);
            ui->yearlySubscriptionButton->setSubTitle(productInfo.description);
            ui->yearlySubscriptionButton->setAccessoryTitle(productInfo.price);
            ui->yearlySubscriptionButton->setObjectName(productInfo.identifier);
        }
    }
}

void InAppDialog::on_monthlySubscriptionButton_clicked()
{
    LOG_DEBUG() << "make payment: " << ui->yearlySubscriptionButton->objectName().toUtf8().constData();
    inapp_add_payment(ui->monthlySubscriptionButton->objectName().toUtf8().constData(), inAppCallback, (void*)this);
}

void InAppDialog::on_triMonthSubscriptionButton_clicked()
{
    LOG_DEBUG() << "make payment: " << ui->yearlySubscriptionButton->objectName().toUtf8().constData();
    inapp_add_payment(ui->triMonthSubscriptionButton->objectName().toUtf8().constData(), inAppCallback, (void*)this);
}

void InAppDialog::on_yearlySubscriptionButton_clicked()
{
    LOG_DEBUG() << "make payment: " << ui->yearlySubscriptionButton->objectName().toUtf8().constData();
    inapp_add_payment(ui->yearlySubscriptionButton->objectName().toUtf8().constData(), inAppCallback, (void*)this);
}

void InAppDialog::on_restorePurchaseButton_clicked()
{
    LOG_DEBUG() << "restore purchase";
    inapp_restore_purchase(inAppCallback, (void*)this);
}

void InAppDialog::on_privacyPolicyButton_clicked()
{
    QDesktopServices::openUrl(QUrl("http://moviemator.net/privacy.html"));
}

void InAppDialog::on_termOfUseButton_clicked()
{
    QDesktopServices::openUrl(QUrl("http://moviemator.net/terms-of-service.html"));
}

void InAppDialog::on_useFreeVersionButton_clicked()
{
    this->reject();
}
