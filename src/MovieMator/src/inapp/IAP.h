//
//  IAP.h
//  minetest-ios
//
//  Created by ZhangJie on 9/9/15.
//  Copyright (c) 2015 EffectMatrix Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

#define MONTHLY_SUBSCRIPTION @"net.moviemator.moviemator.subscription_one_month"
#define THRERE_MONTH_SUBSCRIPTION @"net.moviemator.moviemator.subscription_three_months"
#define YEARLY_SUBSCRIPTION @"net.moviemator.moviemator.subscription_one_year"

#define kSUBSCRIPTION_EXPIRATION @"subcription_expiration_date"


@interface IAP : NSObject <SKProductsRequestDelegate, SKPaymentTransactionObserver, SKRequestDelegate>
{
    BOOL m_bFirstRecharge;
}


@property (nonatomic, retain) NSArray *products;


+ (IAP *)sharedInstance;

- (BOOL)canMakePayments;

- (void)fetchProductInformation;

- (NSString *)getLocalPriceForProduct:(NSString *)productIdentifier;

- (float)getChinesePriceForProduct:(NSString *)productIdentifier;

- (void)buyProduct:(NSString *)productIdentifier;

- (void)restoreCompletedTransactions;

- (void)verifyReceiptOnThread;

- (void)verifyRestoreOnThread;

//返回值：0成功；-1失败。
- (int)verifyInappReceiptLocally;

- (void)refreshReceipt;

- (int)hasValidSubscription;
@end
