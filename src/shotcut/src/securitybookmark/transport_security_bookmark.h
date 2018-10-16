//
//  transport_security_bookmark.h
//  security_bookmark
//
//  Created by 东彪高 on 11/24/16.
//  Copyright © 2016 etinysoft. All rights reserved.
//

#ifndef transport_security_bookmark_h
#define transport_security_bookmark_h

int create_security_bookmark(const char *path);
int resolve_security_bookmark();
bool has_access_permission(const char *path);

#endif /* transport_security_bookmark_h */
