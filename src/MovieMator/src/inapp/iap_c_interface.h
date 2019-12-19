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


#ifndef IAP_C_INTERFACE_H
#define IAP_C_INTERFACE_H

#define STRING_LENGTH 128
typedef struct {
    char identifier[STRING_LENGTH];
    char name[STRING_LENGTH];
    char description[STRING_LENGTH];
    char price[STRING_LENGTH];
} INAPP_PRODUCT_INFO;

//result：内购结果，0成功，-1失败。
typedef void (*InAppPurchaseCallback)(void *caller, int result);

//用户是否允许购买
//允许返回1，不允许返回0
int inapp_is_authorized_for_payments();

//拉取产品信息
void inapp_fetch_product_infomation();

//返回产品数量
int inapp_get_product_count();

//获取产品信息
//返回产品信息数组，失败返回NULL
INAPP_PRODUCT_INFO *inapp_get_product_information();

//购买产品
void inapp_add_payment(const char *product_identifer, InAppPurchaseCallback inapp_callback, void *callbackObj);

//恢复购买
void inapp_restore_purchase(InAppPurchaseCallback inapp_callback, void *callbackObj);

//验证收据信息
//返回值：0成功；-1失败。
int inapp_verify_receipt();

//刷新收据
void inapp_refresh_receipt();

#endif //IAP_C_INTERFACE_H
