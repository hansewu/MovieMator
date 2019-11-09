//
//  IAP.m
//  minetest-ios
//
//  Created by ZhangJie on 9/9/15.
//  Copyright (c) 2015 EffectMatrix Inc. All rights reserved.
//

#import "IAP.h"
//#include "EventAdapter.h"
//#import "ResourceManager.h"
//#import "BaiduGameStat.h"
//#import "CommFunction.h"




@implementation IAP

#define SERVER_URL @"http://www.totalrar.com/iap/iap.php"
//#define SERVER_URL @"http://192.168.2.103/iap/iap.php"


+ (IAP *)sharedInstance
{
    static IAP *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[IAP alloc] init];
        
        // Do any other initialisation stuff here
        
        // add an observer to the payment queue, so that we can catch any payment
        // changes
        [[SKPaymentQueue defaultQueue] addTransactionObserver:sharedInstance];
        
    });
    return sharedInstance;
}


- (BOOL)canMakePayments
{
    if ([SKPaymentQueue canMakePayments])
    {
        return TRUE;
    }
    
    NSLog(@"失败，用户禁止应用内付费购买.");
    
    return FALSE;
}

-(void)fetchProductInformation
{
    // Create a set for your product identifier
    //NSSet *mySet = [NSSet setWithObject:your_product_identifier];
    
    NSSet * mySet = nil;
    
    mySet = [NSSet setWithArray:[NSArray arrayWithObjects:MONTHLY_SUBSCRIPTION, THRERE_MONTH_SUBSCRIPTION, YEARLY_SUBSCRIPTION, nil]];
    
    // Create a product request object and initialize it with the above set
    SKProductsRequest *request = [[SKProductsRequest alloc] initWithProductIdentifiers:mySet];
    
    request.delegate = self;
    
    // Send the request to the App Store
    [request start];
}


//Get the App Store's response
- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response
{
    NSArray *myProducts = response.products;
    if (myProducts.count == 0)
    {
        NSLog(@"无法获取产品信息，购买失败。");
        return;
    }
    
    if ([response.products count] > 0)
    {
        // Use availableProducts to populate your UI
        NSArray * skProducts = response.products;
        for (SKProduct * skProduct in skProducts)
        {
            NSLog(@"Found product: %@ %@ %@ %0.2f",
                  skProduct.productIdentifier,
                  skProduct.localizedTitle,
                  skProduct.localizedDescription,
                  skProduct.price.floatValue);
        }
        
        
        self.products = response.products;
    }
    
    [request release];
}

- (NSString *)getLocalPriceForProduct:(NSString *)productIdentifier
{
    if (self.products )
    {
        
        for (SKProduct * skProduct in self.products)
        {
            if ([skProduct.productIdentifier isEqualToString:productIdentifier])
            {
                
                NSNumberFormatter *numberFormatter = [[[NSNumberFormatter alloc] init] autorelease];
                [numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
                [numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
                [numberFormatter setLocale:skProduct.priceLocale];
                NSString *formattedPrice = [numberFormatter stringFromNumber:skProduct.price];
                
                return formattedPrice;
            }
        }
        
    }
    
    return @"";
}

- (float)getChinesePriceForProduct:(NSString *)productIdentifier
{
    if (self.products ) {
        
        for (SKProduct * skProduct in self.products) {
            if ([skProduct.productIdentifier isEqualToString:productIdentifier]) {
                
                /*
                NSNumberFormatter *numberFormatter = [[[NSNumberFormatter alloc] init] autorelease];
                [numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
                [numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
                NSLocale *curLocal = [[NSLocale alloc]initWithLocaleIdentifier:@"zh-Hans"];
                [numberFormatter setLocale:curLocal];
                [curLocal release];
                
                NSString *formattedPrice = [numberFormatter stringFromNumber:skProduct.price];
                
                // Intermediate
                NSMutableString *numberString = [[[NSMutableString alloc] init] autorelease];
                NSString *tempStr;
                NSScanner *scanner = [NSScanner scannerWithString:formattedPrice];
                NSCharacterSet *numbers = [NSCharacterSet characterSetWithCharactersInString:@"0123456789."];
                
                while (![scanner isAtEnd]) {
                    // Throw away characters before the first number.
                    [scanner scanUpToCharactersFromSet:numbers intoString:NULL];
                    
                    // Collect numbers.
                    [scanner scanCharactersFromSet:numbers intoString:&tempStr];
                    [numberString appendString:tempStr];
                    tempStr = @"";  
                }  
                // Result.  
                return [numberString floatValue];
                 */                
            }
        }
        
    }
    
    return 0;
}


- (void)buyProduct:(NSString *)productIdentifier
{
    if (self.products) {
        
        for (SKProduct * skProduct in self.products) {
            if ([skProduct.productIdentifier isEqualToString:productIdentifier]) {
                
                SKPayment * payment = [SKPayment paymentWithProduct:skProduct];
                [[SKPaymentQueue defaultQueue] addPayment:payment];
                
            }
        }
    }
}

- (void)restoreCompletedTransactions {
    [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
}


- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions {
    for (SKPaymentTransaction *transaction in transactions)
    {
        switch (transaction.transactionState)
        {
            case SKPaymentTransactionStatePurchased:
                [self completeTransaction:transaction];
                break;
            case SKPaymentTransactionStateFailed:
                [self failedTransaction:transaction];
                break;
            case SKPaymentTransactionStateRestored:
                [self restoreTransaction:transaction];
                break;
            case SKPaymentTransactionStatePurchasing:                      NSLog(@"商品添加进列表");
                break;
            default:
                break;
        }
    }
    
}

- (void)completeTransaction:(SKPaymentTransaction *)transaction {
    // Your application should implement these two methods.
    
    NSLog(@"buy: %@", transaction.payment.productIdentifier);
    NSLog(@"transactionIdentifier: %@", transaction.transactionIdentifier);
    
    NSString * productIdentifier = transaction.payment.productIdentifier;
    
//    NSURL *receiptURL = [[NSBundle mainBundle] appStoreReceiptURL];
//    NSData *receiptData = [NSData dataWithContentsOfURL:receiptURL];
    
    if ([productIdentifier length] > 0) {
        
        NSLog(@"开启productIdentifier功能");
        int nCoins = 0;
        
        NSTimeInterval time = [[NSDate date] timeIntervalSince1970];
        
//        if ([productIdentifier isEqualToString:VIP_PRODUCT_1]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:YES forKey:VIP_PRODUCT_1];
//            [storage synchronize];
//        }else if ([productIdentifier isEqualToString:VIP_PRODUCT_2]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:YES forKey:VIP_PRODUCT_2];
//            [storage setDouble:time forKey:[VIP_PRODUCT_2 stringByAppendingString:@"Timer"]];
//            [storage synchronize];
//        }else if ([productIdentifier isEqualToString:VIP_PRODUCT_3]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:YES forKey:VIP_PRODUCT_3];
//            [storage setDouble:time forKey:[VIP_PRODUCT_3 stringByAppendingString:@"Timer"]];
//            [storage synchronize];
//        }else if ([productIdentifier isEqualToString:VIP_PRODUCT_4]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:YES forKey:VIP_PRODUCT_4];
//            [storage setDouble:time forKey:[VIP_PRODUCT_4 stringByAppendingString:@"Timer"]];
//            [storage synchronize];
//        }else if ([productIdentifier isEqualToString:MOREWORLDS_PRODUCT]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:YES forKey:MOREWORLDS_PRODUCT];
//            [storage synchronize];
//        }else if ([productIdentifier isEqualToString:SAVEWORLD_PRODUCT]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:YES forKey:SAVEWORLD_PRODUCT];
//            [storage synchronize];
//        }else if ([productIdentifier isEqualToString:MUSEUM_ASIAWORLD_PRODUCT]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:YES forKey:MUSEUM_ASIAWORLD_PRODUCT];
//            [storage synchronize];
//        }else if ([productIdentifier isEqualToString:MUSEUM_AMERICAWORLD_PRODUCT]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:YES forKey:MUSEUM_AMERICAWORLD_PRODUCT];
//            [storage synchronize];
//        }else if ([productIdentifier isEqualToString:MUSEUM_EUROPEWORLD_PRODUCT]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:YES forKey:MUSEUM_EUROPEWORLD_PRODUCT];
//            [storage synchronize];
//        }else if ([productIdentifier isEqualToString:MUSEUM_MUSEUMSERVER_PRODUCT]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:YES forKey:MUSEUM_MUSEUMSERVER_PRODUCT];
//            [storage synchronize];
//        }else if ([productIdentifier isEqualToString:ENTRANCE_MUSEUM_PRODUCT]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:YES forKey:ENTRANCE_MUSEUM_PRODUCT];
//            [storage synchronize];
//        }else if ([productIdentifier isEqualToString:ENTRANCE_ONLINE_PRODUCT]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:YES forKey:ENTRANCE_ONLINE_PRODUCT];
//            [storage synchronize];
//        }else {
//            [self grantProduct:productIdentifier :nCoins];
//        }

        //[self addPurchaseRecord:transaction.payment.productIdentifier :transaction.transactionIdentifier :transaction.transactionReceipt :nCoins];
        
        // 向自己的服务器验证购买凭证
        [self verifyReceiptOnThread];
    }
    
    // Remove the transaction from the payment queue.
    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
    
    [[NSNotificationCenter defaultCenter] postNotificationName:@"FINISH_TRANSACTION" object:nil];
}

- (void)failedTransaction:(SKPaymentTransaction *)transaction {
//    if(transaction.error.code != SKErrorPaymentCancelled) {
//        NSLog(@"购买失败 %ld", (long)transaction.error.code);
//    } else {
//        NSLog(@"用户取消交易");
//        if ([transaction.payment.productIdentifier isEqualToString:SAVEWORLD_PRODUCT]) {
//            [[NSNotificationCenter defaultCenter] postNotificationName:@"CLEAN_WORLDS" object:nil];
//        }
//    }
//    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
//    [[NSNotificationCenter defaultCenter] postNotificationName:@"FINISH_TRANSACTION" object:nil];

}

- (void)restoreTransaction:(SKPaymentTransaction *)transaction {
//    // 对于已购商品，处理恢复购买的逻辑
    
//    NSLog(@"restore: %@", transaction.originalTransaction.payment.productIdentifier);
//    NSLog(@"transactionIdentifier: %@", transaction.originalTransaction.transactionIdentifier);
    
//    NSLog(@"恢复productIdentifier功能");
//    if ([transaction.originalTransaction.payment.productIdentifier isEqualToString:VIP_PRODUCT_1]) {
//        NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//        [storage setBool:YES forKey:VIP_PRODUCT_1];
//        [storage synchronize];
//    }else if ([transaction.originalTransaction.payment.productIdentifier isEqualToString:MOREWORLDS_PRODUCT]) {
//        NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//        [storage setBool:YES forKey:MOREWORLDS_PRODUCT];
//        [storage synchronize];
//    }else if ([transaction.originalTransaction.payment.productIdentifier isEqualToString:MUSEUM_ASIAWORLD_PRODUCT]) {
//        NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//        [storage setBool:YES forKey:MUSEUM_ASIAWORLD_PRODUCT];
//        [storage synchronize];
//    }else if ([transaction.originalTransaction.payment.productIdentifier isEqualToString:MUSEUM_AMERICAWORLD_PRODUCT]) {
//        NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//        [storage setBool:YES forKey:MUSEUM_AMERICAWORLD_PRODUCT];
//        [storage synchronize];
//    }else if ([transaction.originalTransaction.payment.productIdentifier isEqualToString:MUSEUM_EUROPEWORLD_PRODUCT]) {
//        NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//        [storage setBool:YES forKey:MUSEUM_EUROPEWORLD_PRODUCT];
//        [storage synchronize];
//    }else if ([transaction.originalTransaction.payment.productIdentifier isEqualToString:MUSEUM_MUSEUMSERVER_PRODUCT]) {
//        NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//        [storage setBool:YES forKey:MUSEUM_MUSEUMSERVER_PRODUCT];
//        [storage synchronize];
//    }else if ([transaction.originalTransaction.payment.productIdentifier isEqualToString:SAVEWORLD_PRODUCT]) {
//        NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//        [storage setBool:YES forKey:SAVEWORLD_PRODUCT];
//        [storage synchronize];
//    }else if ([transaction.originalTransaction.payment.productIdentifier isEqualToString:ENTRANCE_MUSEUM_PRODUCT]) {
//        NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//        [storage setBool:YES forKey:ENTRANCE_MUSEUM_PRODUCT];
//        [storage synchronize];
//    }
//    else if ([transaction.originalTransaction.payment.productIdentifier isEqualToString:ENTRANCE_ONLINE_PRODUCT]) {
//        NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//        [storage setBool:YES forKey:ENTRANCE_ONLINE_PRODUCT];
//        [storage synchronize];
//    }
    
//    [self addRestoreRecord:transaction.originalTransaction.payment.productIdentifier :transaction.originalTransaction.transactionIdentifier];
//    [self verifyRestoreOnThread];
    
//    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
    
//    [[NSNotificationCenter defaultCenter] postNotificationName:@"FINISH_TRANSACTION" object:nil];
}

- (void)verifyReceiptOnThread
{
    NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
    NSMutableArray *records = [storage objectForKey:@"PurchaseRecord"];
    
    if (records) {
        for (NSArray *record in records) {
            
            [NSThread detachNewThreadSelector:@selector(verifyReceipt:) toTarget:self withObject:record];
            
        }
    }
    
}

- (void) verifyReceipt:(NSArray *)arr
{
//    if ([arr count] != 4) {
//        return;
//    }
    
//    NSString *productID = [arr objectAtIndex:0];
//    NSString *transactionID = [arr objectAtIndex:1];
//    NSData *transactionReceipt = [arr objectAtIndex:2];
//    NSNumber *coins = [arr objectAtIndex:3];

    
    
//    NSString *encodeStr = [transactionReceipt base64EncodedStringWithOptions:0];
    
//    NSString *payload = [NSString stringWithFormat:@"productID=%@&transactionID=%@&transactionReceipt=%@", productID, transactionID, encodeStr];
//    NSData *payloadData = [payload dataUsingEncoding:NSUTF8StringEncoding];
    
    
//    // 初始化请求, 这里是变长的, 方便扩展
//    NSMutableURLRequest *request = [[NSMutableURLRequest alloc] init];
    
//    // 设置
//    request.cachePolicy = NSURLRequestReloadIgnoringCacheData;
//    [request setURL:[NSURL URLWithString:SERVER_URL]];
//    [request setHTTPMethod:@"POST"];
//    NSString *contentLength = [NSString stringWithFormat:@"%lu", (unsigned long)[payloadData length]];
//    [request setValue:contentLength forHTTPHeaderField:@"Content-Length"];
//    [request setHTTPBody:payloadData];

    
//    // 发送同步请求, data就是返回的数据
//    NSError *error = nil;
//    NSData *data = [NSURLConnection sendSynchronousRequest:request returningResponse:nil error:&error];
//    if (data == nil) {
//        NSLog(@"send request failed: %@", error);
//        return;
//    }
    
//    NSString *response = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
//    NSLog(@"verifyReceipt response: %@", response);
    
    
//    if ([response containsString:@"fail purchase"]) {
        
//        NSLog(@"撤销开放的in app 功能！！！");
        
//        if ([productID isEqualToString:VIP_PRODUCT_1]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:NO forKey:VIP_PRODUCT_1];
//            [storage synchronize];
//        }else if ([productID isEqualToString:VIP_PRODUCT_2]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:NO forKey:VIP_PRODUCT_2];
//            [storage synchronize];
//        } else if ([productID isEqualToString:VIP_PRODUCT_3]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:NO forKey:VIP_PRODUCT_3];
//            [storage synchronize];
//        }else if ([productID isEqualToString:VIP_PRODUCT_4]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:NO forKey:VIP_PRODUCT_4];
//            [storage synchronize];
//        }else if ([productID isEqualToString:MOREWORLDS_PRODUCT]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:NO forKey:MOREWORLDS_PRODUCT];
//            [storage synchronize];
//        }else if ([productID isEqualToString:MUSEUM_ASIAWORLD_PRODUCT]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:NO forKey:MUSEUM_ASIAWORLD_PRODUCT];
//            [storage synchronize];
//        }else if ([productID isEqualToString:MUSEUM_AMERICAWORLD_PRODUCT]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:NO forKey:MUSEUM_AMERICAWORLD_PRODUCT];
//            [storage synchronize];
//        }else if ([productID isEqualToString:MUSEUM_EUROPEWORLD_PRODUCT]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:NO forKey:MUSEUM_EUROPEWORLD_PRODUCT];
//            [storage synchronize];
//        }else if ([productID isEqualToString:MUSEUM_MUSEUMSERVER_PRODUCT]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:NO forKey:MUSEUM_MUSEUMSERVER_PRODUCT];
//            [storage synchronize];
//        }else if ([productID isEqualToString:SAVEWORLD_PRODUCT]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:NO forKey:SAVEWORLD_PRODUCT];
//            [storage synchronize];
            
//            [[NSNotificationCenter defaultCenter] postNotificationName:@"CLEAN_WORLDS" object:nil];
//        }else if ([productID isEqualToString:ENTRANCE_MUSEUM_PRODUCT]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:NO forKey:ENTRANCE_MUSEUM_PRODUCT];
//            [storage synchronize];
//        }else if ([productID isEqualToString:ENTRANCE_ONLINE_PRODUCT]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:NO forKey:ENTRANCE_ONLINE_PRODUCT];
//            [storage synchronize];
//        }else {
//            [self removeProduct:productID :[coins intValue]];
//        }
        
//    }else {
//            if ([productID isEqualToString:COIN_PRODUCT_0]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:YES forKey:@"OneRMBBuy"];
//            [storage synchronize];
            
//            [self performSelectorOnMainThread:@selector(onShowShoppingView) withObject:nil waitUntilDone:NO];
            

//        }else if ([productID isEqualToString:MOREWORLDS_PRODUCT]) {
//            [[NSNotificationCenter defaultCenter] postNotificationName:@"RELOAD_WORLDS" object:nil];
//        }
        

        
//        if (m_bFirstRecharge) {
//        }
        
//        NSArray *products = [NSArray arrayWithObjects:COIN_PRODUCT_1, COIN_PRODUCT_2, COIN_PRODUCT_3, COIN_PRODUCT_4, COIN_PRODUCT_5, COIN_PRODUCT_6, COIN_PRODUCT_7, COIN_PRODUCT_8, COIN_PRODUCT_0, COIN_PRODUCT_N, nil];
        
//        if ([products containsObject:productID]) {
            
//            NSDateFormatter *formatter = [[[NSDateFormatter alloc] init] autorelease];
//            [formatter setDateStyle:NSDateFormatterMediumStyle];
//            [formatter setTimeStyle:NSDateFormatterShortStyle];
//            [formatter setDateFormat:@"YYYY-MM-dd-HH-mm-ss"];
            
//            NSTimeZone* timeZone = [NSTimeZone timeZoneWithName:@"Asia/Shanghai"];
//            [formatter setTimeZone:timeZone];

//            NSString *nowtimeStr = [formatter stringFromDate:[NSDate date]];

//            float fPrice = [self getChinesePriceForProduct:productID];
//            NSString *newRecord = [nowtimeStr stringByAppendingFormat:@":%f;", fPrice];


//           //NSString *record = [NSString stringWithUTF8String:EventAdapter::GetInstance().getPlayerChargeRecord().c_str()];

//            //record = [record stringByAppendingString:newRecord];

//            //NSString *charInfo = [NSString stringWithFormat:@"/record charge_info %@", record];

//            //EventAdapter::GetInstance().setChatIntoClient(charInfo.UTF8String);
//        }
//    }
    
//    [self removePurchaseRecord:productID :transactionID];
    
//    [request release];
//    [response release];
    
}


- (void)verifyRestoreOnThread
{
    NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
    NSMutableArray *records = [storage objectForKey:@"RestoreRecord"];
    
    if (records) {
        for (NSArray *record in records) {
            
            [NSThread detachNewThreadSelector:@selector(verifyRestore:) toTarget:self withObject:record];
            
        }
    }
    
}

- (void) verifyRestore:(NSArray *)arr
{
//    if ([arr count] != 2) {
//        return;
//    }
    
//    NSString *productID = [arr objectAtIndex:0];
//    NSString *transactionID = [arr objectAtIndex:1];
    
    
//    NSString *payload = [NSString stringWithFormat:@"productID=%@&transactionID=%@", productID, transactionID];
//    NSData *payloadData = [payload dataUsingEncoding:NSUTF8StringEncoding];
    
    
//    // 初始化请求, 这里是变长的, 方便扩展
//    NSMutableURLRequest *request = [[NSMutableURLRequest alloc] init];
    
//    // 设置
//    request.cachePolicy = NSURLRequestReloadIgnoringCacheData;
//    [request setURL:[NSURL URLWithString:SERVER_URL]];
//    [request setHTTPMethod:@"POST"];
//    NSString *contentLength = [NSString stringWithFormat:@"%lu", (unsigned long)[payloadData length]];
//    [request setValue:contentLength forHTTPHeaderField:@"Content-Length"];
//    [request setHTTPBody:payloadData];
    
    
//    // 发送同步请求, data就是返回的数据
//    NSError *error = nil;
//    NSData *data = [NSURLConnection sendSynchronousRequest:request returningResponse:nil error:&error];
//    if (data == nil) {
//        NSLog(@"send request failed: %@", error);
//        return;
//    }
    
//    NSString *response = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
//    NSLog(@"verifyRestore response: %@", response);
    
    
//    if ([response containsString:@"fail restore"]) {
        
//        NSLog(@"撤销开放的in app 功能！！！");
//        if ([productID isEqualToString:VIP_PRODUCT_1]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:NO forKey:VIP_PRODUCT_1];
//            [storage synchronize];
//        }else if ([productID isEqualToString:MOREWORLDS_PRODUCT]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:NO forKey:MOREWORLDS_PRODUCT];
//            [storage synchronize];
//        }else if ([productID isEqualToString:MUSEUM_ASIAWORLD_PRODUCT]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:NO forKey:MUSEUM_ASIAWORLD_PRODUCT];
//            [storage synchronize];
//        }else if ([productID isEqualToString:MUSEUM_AMERICAWORLD_PRODUCT]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:NO forKey:MUSEUM_AMERICAWORLD_PRODUCT];
//            [storage synchronize];
//        }else if ([productID isEqualToString:MUSEUM_EUROPEWORLD_PRODUCT]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:NO forKey:MUSEUM_EUROPEWORLD_PRODUCT];
//            [storage synchronize];
//        }else if ([productID isEqualToString:MUSEUM_MUSEUMSERVER_PRODUCT]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:NO forKey:MUSEUM_MUSEUMSERVER_PRODUCT];
//            [storage synchronize];
//        }else if ([productID isEqualToString:SAVEWORLD_PRODUCT]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:NO forKey:SAVEWORLD_PRODUCT];
//            [storage synchronize];
            
//            [[NSNotificationCenter defaultCenter] postNotificationName:@"CLEAN_WORLDS" object:nil];
//        }else if ([productID isEqualToString:ENTRANCE_MUSEUM_PRODUCT]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:NO forKey:ENTRANCE_MUSEUM_PRODUCT];
//            [storage synchronize];
//        }else if ([productID isEqualToString:ENTRANCE_ONLINE_PRODUCT]) {
//            NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
//            [storage setBool:NO forKey:ENTRANCE_ONLINE_PRODUCT];
//            [storage synchronize];
//        }
//    }else {
//        if ([productID isEqualToString:MOREWORLDS_PRODUCT]) {
//            [[NSNotificationCenter defaultCenter] postNotificationName:@"RELOAD_WORLDS" object:nil];
//        }
//    }
    
//    [self removeRestoreRecord:productID :transactionID];
    
//    [request release];
//    [response release];
    
}


- (void)addPurchaseRecord:(NSString *)productID :(NSString *)transactionID :(NSData *)transactionReceipt :(int)nCoins
{
    NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
    NSArray *records = [storage objectForKey:@"PurchaseRecord"];
    NSArray *newRecord = [NSArray arrayWithObjects:productID, transactionID, transactionReceipt, [NSNumber numberWithInt:nCoins], nil];
    
    NSMutableArray *newRecords = nil;
    
    if (records) {
        newRecords = [NSMutableArray arrayWithArray:records];
        [newRecords addObject:newRecord];
    }else{
        newRecords = [NSMutableArray arrayWithObject:newRecord];
    }
    
    [storage setObject:newRecords forKey:@"PurchaseRecord"];
    [storage synchronize];
    
}

- (void)removePurchaseRecord:(NSString *)productID :(NSString *)transactionID
{
    NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
    NSMutableArray *records = [storage objectForKey:@"PurchaseRecord"];
    NSMutableArray *newRecords = [NSMutableArray array];
    
    if (records) {
        for (NSArray *record in records) {
            
            if ([[record objectAtIndex:0]isEqualToString:productID]
                && [[record objectAtIndex:1]isEqualToString:transactionID]) {
            }else{
                [newRecords addObject:record];
            }
        }
        
        [storage setObject:newRecords forKey:@"PurchaseRecord"];
        [storage synchronize];
    }
    
    
}

- (void)addRestoreRecord:(NSString *)productID :(NSString *)transactionID
{
    NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
    NSArray *records = [storage objectForKey:@"RestoreRecord"];
    NSArray *newRecord = [NSArray arrayWithObjects:productID, transactionID, nil];
    
    NSMutableArray *newRecords = nil;
    
    if (records) {
        newRecords = [NSMutableArray arrayWithArray:records];
        [newRecords addObject:newRecord];
    }else{
        newRecords = [NSMutableArray arrayWithObject:newRecord];
    }
    
    [storage setObject:newRecords forKey:@"RestoreRecord"];
    [storage synchronize];
    

}

- (void)removeRestoreRecord:(NSString *)productID :(NSString *)transactionID
{
    NSUserDefaults *storage = [NSUserDefaults standardUserDefaults];
    NSMutableArray *records = [storage objectForKey:@"RestoreRecord"];
    NSMutableArray *newRecords = [NSMutableArray array];
    
    if (records) {
        for (NSArray *record in records) {
            
            if ([[record objectAtIndex:0]isEqualToString:productID]
                && [[record objectAtIndex:1]isEqualToString:transactionID]) {
            }else{
                [newRecords addObject:record];
            }
        }
        
        [storage setObject:newRecords forKey:@"RestoreRecord"];
        [storage synchronize];
    }

}

-(void)grantProduct:(NSString *)productIdentifier :(int &)nCoins
{
//    NSArray *products = [NSArray arrayWithObjects:COIN_PRODUCT_1, COIN_PRODUCT_2, COIN_PRODUCT_3, COIN_PRODUCT_4, COIN_PRODUCT_5, COIN_PRODUCT_6, COIN_PRODUCT_7, COIN_PRODUCT_8, COIN_PRODUCT_0, COIN_PRODUCT_N, nil];
    
//    if ([products containsObject:productIdentifier]) {
//    }
}

-(void)removeProduct:(NSString *)productIdentifier :(int)nCoins
{    
}

- (void)onShowShoppingView
{
    [[NSNotificationCenter defaultCenter] postNotificationName:@"ShowShoppingView" object:nil userInfo:nil];
}


@end
