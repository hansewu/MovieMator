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
#ifndef INAPPDIALOG_H
#define INAPPDIALOG_H

#include <QDialog>
#include "../inapp/iap_c_interface.h"


namespace Ui {
class InAppDialog;
}

class InAppDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InAppDialog(QWidget *parent = 0);
    ~InAppDialog();

    void setInAppProductInfo(INAPP_PRODUCT_INFO *pProductInfos, int nProductCount);

private slots:
    void on_monthlySubscriptionButton_clicked();

    void on_triMonthSubscriptionButton_clicked();

    void on_yearlySubscriptionButton_clicked();

    void on_restorePurchaseButton_clicked();

    void on_privacyPolicyButton_clicked();

    void on_termOfUseButton_clicked();

    void on_useFreeVersionButton_clicked();

private:
    Ui::InAppDialog *ui;
};

#endif // INAPPDIALOG_H
