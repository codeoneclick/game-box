//
//  application_delegate_tvos.mm
//  indi2dEngine-Core
//
//  Created by Sergey Sergeev on 5/3/13.
//  Copyright (c) 2013 Sergey Sergeev. All rights reserved.
//

#include "application_delegate_tvos.h"
#include "game_controller_tvos.h"

@implementation application_delegate_tvos

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    game_controller_tvos *vc = [[game_controller_tvos alloc] init];
    UINavigationController* navigation_controller = [[UINavigationController alloc] initWithRootViewController:vc];
    [navigation_controller setNavigationBarHidden:YES animated:NO];
    self.window.backgroundColor = [UIColor blackColor];
    [self.window setRootViewController:navigation_controller];
    [self.window makeKeyAndVisible];
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{

}

- (void)applicationDidEnterBackground:(UIApplication *)application
{

}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
  
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{

}

- (void)applicationWillTerminate:(UIApplication *)application
{
   
}

@end
