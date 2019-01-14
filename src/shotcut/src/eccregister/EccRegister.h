//
//  EccRegister.h
//  EccTest
//
//  Created by Chin ping Hsu on 3/4/15.
//  Copyright (c) 2015 Chin ping Hsu. All rights reserved.
//
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

#import <Foundation/Foundation.h>
#include "transport_ecc_register.h"



@interface EccRegister : NSObject


/*
 返回值： -1 — 表示注册码无效
 0  —  表示注册码有效
 
 参数strRegisterCode: 注册码字符串（不带‘-’）
 */
+(RegistrationType)CheckRegisterInfo:(NSString*)strRegisterCode;
+(RegistrationType)registration:(NSString *)strRegisterCode;
@end
