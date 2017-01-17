//
//  main.m
//  gbDemo
//
//  Created by sergey.sergeev on 8/11/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#if defined(__IOS__)

#include <UIKit/UIKit.h>
#include "application_delegate_ios.h"

int main(int argc, char *argv[])
{
    @autoreleasepool
    {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([application_delegate_ios class]));
    }
}

#elif defined(__OSX__)

#include <Cocoa/Cocoa.h>

int main(int argc, const char * argv[])
{
    return NSApplicationMain(argc, argv);
}

#endif