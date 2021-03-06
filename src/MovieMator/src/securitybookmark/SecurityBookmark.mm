//
//  SecurityBookmark.m
//  security_bookmark
//
//  Created by 东彪高 on 11/24/16.
//  Copyright © 2016 etinysoft. All rights reserved.
//
/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
 * Author: wyl <wyl@pylwyl.local>
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

#import "SecurityBookmark.h"
#import "transport_security_bookmark.h"

@implementation SecurityBookmark
+ (NSString *)bookmarkDataPlist
{
    NSString *applicationDataPlist = nil;
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
        if ([paths count] > 0) {
//            applicationDataPlist = [[NSString alloc] initWithFormat:@"%@/%@", [paths objectAtIndex:0], @"etinysoft/bookmarks.plist"];
            applicationDataPlist = [[NSString alloc] initWithFormat:@"%@/%@", [paths objectAtIndex:0], @"effectmatrix/bookmarks.plist"];
            NSMutableDictionary * prefs;
            prefs = [NSMutableDictionary dictionaryWithContentsOfFile: [applicationDataPlist stringByExpandingTildeInPath]];
            if (prefs == nil)
            {
                prefs = [NSMutableDictionary dictionary];
               [prefs writeToFile:[applicationDataPlist stringByExpandingTildeInPath] atomically: TRUE];
            }
        }
        return applicationDataPlist;
}


+ (int)createBookmark:(NSString *)path
{
    int result = 0;
    @autoreleasepool {


    NSURL *url = [NSURL fileURLWithPath:path];
    NSError *error = nil;
    NSData *bookmarkData = [url bookmarkDataWithOptions:NSURLBookmarkCreationWithSecurityScope includingResourceValuesForKeys:nil relativeToURL:nil error:&error];
    if (error)
    {
        result = 1;
    }
    else if(bookmarkData)
    {
        NSString * applicationDataPlist = [SecurityBookmark bookmarkDataPlist];
        NSMutableDictionary * prefs;
        prefs = [NSMutableDictionary dictionaryWithContentsOfFile:applicationDataPlist ];
        [prefs setObject:bookmarkData forKey:path];
        [prefs writeToFile:[applicationDataPlist stringByExpandingTildeInPath] atomically: TRUE];
    }
    else
        result = 1;

    }
    return result;
}

+ (int)resolveBookmarks
{
    @autoreleasepool {


    NSString * applicationDataPlist = [SecurityBookmark bookmarkDataPlist];
    NSMutableDictionary * prefs;
    prefs = [NSMutableDictionary dictionaryWithContentsOfFile:applicationDataPlist ];
    NSMutableArray *removeKeys = [[NSMutableArray alloc] init];
    for (NSString *key in prefs)
    {
        NSData *bookmarkData = [prefs objectForKey:key];
        NSError *error = nil;

        NSURL *url = [[NSURL URLByResolvingBookmarkData:bookmarkData options:NSURLBookmarkResolutionWithSecurityScope relativeToURL:nil bookmarkDataIsStale:nil error:&error] retain];
        if (url)
        {
            if ([[NSFileManager defaultManager] fileExistsAtPath:reinterpret_cast<NSString *_Nonnull>([url path])])
                [url startAccessingSecurityScopedResource];
            else
                [removeKeys addObject:key];
        }
        else
            [removeKeys addObject:key];
    }
    [prefs removeObjectsForKeys:removeKeys];
    [prefs writeToFile:[applicationDataPlist stringByExpandingTildeInPath] atomically: TRUE];


    }
    return 0;
}


+ (bool)hasAccessPermission:(NSString *)path
{
    BOOL ret = 0;
    @autoreleasepool {


    NSString * applicationDataPlist = [SecurityBookmark bookmarkDataPlist];
    NSMutableDictionary * prefs;
    prefs = [NSMutableDictionary dictionaryWithContentsOfFile:applicationDataPlist ];

    ret = [[prefs allKeys] containsObject: path];

    }
    return ret;
}


@end

int create_security_bookmark(const char *path)
{
    int ret;

    ret = [SecurityBookmark createBookmark: [NSString stringWithUTF8String: path]];
    return ret;
}

int resolve_security_bookmark()
{
    return [SecurityBookmark resolveBookmarks];
}

bool has_access_permission(const char *path)
{
    bool ret;
    ret = [SecurityBookmark hasAccessPermission: [NSString stringWithUTF8String: path]];
    return ret;
}
