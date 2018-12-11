//
//  CEccRegister.h
//

#include "transport_ecc_register.h"
#include <string>

class CEccRegister
{
private:
	CEccRegister(){}

	/*
	 返回值： -1 — 表示注册码无效
	 0  —  表示注册码有效
	 
	 参数strRegisterCode: 注册码字符串（不带‘-’）
	 */
    static RegistrationType checkRegisterInfo(std::string strRegisterCode);
public:
    static RegistrationType registration(std::string strRegisterCode);
};
