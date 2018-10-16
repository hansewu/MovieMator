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
