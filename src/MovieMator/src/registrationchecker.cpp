/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
 * Author: wyl <wyl@pylwyl.local>
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

#include "registrationchecker.h"
#include "settings.h"
#include "eccregister/transport_ecc_register.h"


RegistrationChecker::RegistrationChecker(QObject *parent) : QObject(parent)
  ,m_registrationType(Registration_None)
  ,m_userEmail(QString(""))
  ,m_licenseCode(QString(""))
{
    printf("ddf");
}

//RegistrationChecker::~RegistrationChecker()
//{

//}

RegistrationChecker& RegistrationChecker::singleton()
{
    static RegistrationChecker *instance = nullptr;
    if (!instance)
    {
        instance = new RegistrationChecker;
    }
    return *instance;
}

RegistrationType RegistrationChecker::registerWithLicense(QString &email, QString &licenseCode)
{
//    RegistrationType type = Registration_Commercial; //ecc_verify_register_info((char *)licenseCode.toUtf8().constData());
#if (defined(SHARE_VERSION) && defined(MOVIEMATOR_PRO) && (!defined(STEAM))) || defined(MOVIEMATOR_FREE)
         RegistrationType type = ecc_verify_register_info(const_cast<char*>(licenseCode.toUtf8().constData()));
#else
    RegistrationType type = Registration_Commercial;
#endif

    if (type != Registration_None)
    {
        //保存注册信息
        setUserMail(email);
        setRegistrationType(type);
        setLicenseCode(licenseCode);
        Settings.setLicenseCode(licenseCode);
        Settings.setUserEmail(email);
    }
    return type;
}

void RegistrationChecker::readAndCheckRegistrationInfo()
{
    QString license = Settings.licenseCode();
    QString userMail = Settings.userEmail();
    registerWithLicense(userMail, license);
}

QString RegistrationChecker::userMail()
{
    return m_userEmail;
}

void RegistrationChecker::setUserMail(QString &mail)
{
    m_userEmail = mail;
}

QString RegistrationChecker::licenseCode()
{
    return m_licenseCode;
}

void RegistrationChecker::setLicenseCode(QString &licenseCode)
{
    m_licenseCode = licenseCode;
}

RegistrationType RegistrationChecker::registrationType()
{
    return m_registrationType;
}

void RegistrationChecker::setRegistrationType(RegistrationType type)
{
    m_registrationType = type;
}
