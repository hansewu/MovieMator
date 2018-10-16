//
//  EccRegister.h
//  EccTest
//
//  Created by Chin ping Hsu on 3/4/15.
//  Copyright (c) 2015 Chin ping Hsu. All rights reserved.
//

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
