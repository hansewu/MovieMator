//
//  IAP.h
//  minetest-ios
//
//  Created by ZhangJie on 9/9/15.
//  Copyright (c) 2015 EffectMatrix Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

#define MONTHLY_SUBSCRIPTION @"net.moviemator.moviemator.monthly_subscription"
#define THRERE_MONTH_SUBSCRIPTION @"net.moviemator.moviemator.3month_subscription"
#define YEARLY_SUBSCRIPTION @"net.moviemator.moviemator.yearly_subscription"


@interface IAP : NSObject <SKProductsRequestDelegate, SKPaymentTransactionObserver>
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
@end
