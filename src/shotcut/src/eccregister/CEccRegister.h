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
