//
//  SecurityBookmark.h
//  security_bookmark
//
//  Created by 东彪高 on 11/24/16.
//  Copyright © 2016 etinysoft. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface SecurityBookmark : NSObject
+ (int)createBookmark:(NSString *)path;
+ (int)resolveBookmarks;
+ (NSString *)bookmarkDataPlist;
+ (bool)hasAccessPermission:(NSString *)path;
@end
