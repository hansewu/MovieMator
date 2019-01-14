/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
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

#ifndef REGISTRATIONCHECKER_H
#define REGISTRATIONCHECKER_H

#include <QObject>
#include <QString>
#include "eccregister/transport_ecc_register.h"




class RegistrationChecker : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString userMail READ userMail WRITE setUserMail)
    Q_PROPERTY(QString licenseCode READ licenseCode WRITE setLicenseCode)
    Q_PROPERTY(RegistrationType registrationType READ registrationType WRITE setRegistrationType)

public:
    explicit RegistrationChecker(QObject *parent = 0);
//    ~RegistrationChecker();

    static RegistrationChecker& singleton();

    RegistrationType registerWithLicense(QString &email, QString& licenseCode);
    void readAndCheckRegistrationInfo();

    QString userMail();
    void setUserMail(QString &mail);
    QString licenseCode();
    void setLicenseCode(QString &licenseCode);
    RegistrationType registrationType();
    void setRegistrationType(RegistrationType type);

private:
    RegistrationType m_registrationType;
    QString m_userEmail;
    QString m_licenseCode;
};

#define Registration RegistrationChecker::singleton()

#endif // REGISTRATIONCHECKER_H
