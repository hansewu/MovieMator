/*
 * Copyright (c) 2013 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
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

#ifndef CUSTOMPROFILEDIALOG_H
#define CUSTOMPROFILEDIALOG_H

#include <QDialog>

namespace Ui {
class CustomProfileDialog;
}

class CustomProfileDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit CustomProfileDialog(QWidget *parent = nullptr);
    ~CustomProfileDialog();
    QString profileName() const;
    
private slots:
    void on_buttonBox_accepted();
    
private:
    Ui::CustomProfileDialog *ui;
};

#endif // CUSTOMPROFILEDIALOG_H
