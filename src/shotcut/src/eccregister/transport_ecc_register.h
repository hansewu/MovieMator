#ifndef TRANSPORT_ECC_REGISTER_H
#define TRANSPORT_ECC_REGISTER_H


//
typedef enum {
    Registration_None = 1,
    Registration_Personal = 2,
    Registration_Commercial = 3,
    Registration_Family = 4,

} RegistrationType;


RegistrationType ecc_verify_register_info(char *license);

#endif // TRANSPORT_ECC_REGISTER_H
