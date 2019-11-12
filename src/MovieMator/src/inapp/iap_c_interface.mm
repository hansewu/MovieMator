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

#include "iap_c_interface.h"
#include "IAP.h"

//EventAdapter，接收IAP消息，调用回调函数
//@interface IAPEventAdapter: public NSObject
//{
//}
//- (void)setRestoreCallback:(CallbackFunc);
//- (void)setPurchaseCallback:(CallbackFunc);
//- (IAPEventAdapter *)sharedInstance;

//#end



int inapp_is_authorized_for_payments()
{
    return [[IAP sharedInstance] canMakePayments] == YES ? 1 : 0;
}

void inapp_fetch_product_infomation()
{
    [[IAP sharedInstance] fetchProductInformation];
}

//验证收据信息
int inapp_verify_receipt()
{
    return [[IAP sharedInstance] verifyInappReceiptLocally];
}

//返回产品数量
int inapp_get_product_count()
{
    return [IAP sharedInstance].products.count;
}

//获取产品信息
INAPP_PRODUCT_INFO *inapp_get_product_information()
{
    int product_count = [IAP sharedInstance].products.count;
    if (product_count > 0)
    {
        INAPP_PRODUCT_INFO *product_infos = (INAPP_PRODUCT_INFO *)malloc(product_count * sizeof(INAPP_PRODUCT_INFO));
        for (int i = 0; i < product_count; i++)
        {
            SKProduct *skProduct = [[IAP sharedInstance].products objectAtIndex:i];
            strcpy(product_infos[i].identifier, [skProduct.productIdentifier UTF8String]);
            if(skProduct.localizedTitle)
                strcpy(product_infos[i].name, [skProduct.localizedTitle UTF8String]);
            else
                strcpy(product_infos[i].name, "");

            if(skProduct.localizedDescription)
                strcpy(product_infos[i].description, [skProduct.localizedDescription UTF8String]);
            else
                strcpy(product_infos[i].description, "");

            NSString *localPrice = [[IAP sharedInstance] getLocalPriceForProduct:skProduct.productIdentifier];
            strcpy(product_infos[i].price, [localPrice UTF8String]);
        }
        return product_infos;
    }

    return nullptr;
}

//购买产品
void inapp_add_payment(const char *product_identifer/*, callbackfunc*/)
{
    [[IAP sharedInstance] buyProduct: [NSString stringWithUTF8String:product_identifer]];
}

//恢复购买
void inapp_restore_purchase(/*callbackfunc*/)
{
    [[IAP sharedInstance] restoreCompletedTransactions];
}
