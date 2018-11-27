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
    static RegistrationChecker *instance = 0;
    if (!instance)
    {
        instance = new RegistrationChecker;
    }
    return *instance;
}

RegistrationType RegistrationChecker::registerWithLicense(QString &email, QString &licenseCode)
{
//    RegistrationType type = Registration_Commercial; //ecc_verify_register_info((char *)licenseCode.toUtf8().constData());
#if defined(Q_OS_MAC) && defined(SHARE_VERSION) && defined(MOVIEMATOR_PRO)
         RegistrationType type = ecc_verify_register_info((char *)licenseCode.toUtf8().constData());
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
