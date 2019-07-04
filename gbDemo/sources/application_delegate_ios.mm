//
//  application_delegate_ios.mm
//  indi2dEngine-Core
//
//  Created by Sergey Sergeev on 5/3/13.
//  Copyright (c) 2013 Sergey Sergeev. All rights reserved.
//

#include "application_delegate_ios.h"
#include "game_controller_ios.h"
#include "advertisement_provider.h"
#include "game_loop.h"
#include "FCUUID.h"

#import <Fabric/Fabric.h>
#import <Crashlytics/Crashlytics.h>
#import <FBSDKCoreKit/FBSDKCoreKit.h>

@implementation application_delegate_ios

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    [FIRApp configure];
    // NSString* device_id = [FCUUID uuidForDevice];
    
    FIRFirestore *db = [FIRFirestore firestore];
    FIRFirestoreSettings* settings = db.settings;
    settings.timestampsInSnapshotsEnabled = YES;
    db.settings = settings;
    /*__block FIRDocumentReference *reference =
    [[db collectionWithPath:@"users"] addDocumentWithData:@{@"user_id": @1,
                                                            @"rank": @1,
                                                            @"stars": @3} completion:^(NSError * _Nullable error) {
                                                                if (error != nil) {
                                                                    NSLog(@"Error adding document: %@", error);
                                                                } else {
                                                                    NSLog(@"Document added with ID: %@", reference.documentID);
                                                                }
                                                            }];
    
    [[[db collectionWithPath:@"users"] queryWhereField:@"user_id" isEqualTo:@1] getDocumentsWithCompletion:^(FIRQuerySnapshot *snapshot, NSError *error) {
        if (error != nil) {
            NSLog(@"Error getting documents: %@", error);
        } else {
            for (FIRDocumentSnapshot *document in snapshot.documents) {
                NSLog(@"%@ => %@", document.documentID, document.data);
            }
        }
    }];*/
    
    [Fabric with:@[[Crashlytics class]]];
    
    [[FBSDKApplicationDelegate sharedInstance] application:application
                             didFinishLaunchingWithOptions:launchOptions];
    
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    game_controller_ios *vc = [[game_controller_ios alloc] init];
    UINavigationController* navigation_controller = [[UINavigationController alloc] initWithRootViewController:vc];
    [navigation_controller setNavigationBarHidden:YES animated:NO];
    
    game::advertisement_provider::shared_instance()->assign_root_navigation_controller((__bridge void*)navigation_controller);
    game::advertisement_provider::shared_instance()->assign_root_view((__bridge void*)[vc view]);
    
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
    gb::reset_run_loop_timestamp();
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    gb::reset_run_loop_timestamp();
}

- (void)applicationWillTerminate:(UIApplication *)application
{
   
}

- (BOOL)application:(UIApplication *)application
            openURL:(NSURL *)url
            options:(NSDictionary<UIApplicationOpenURLOptionsKey,id> *)options {
    
    BOOL handled = [[FBSDKApplicationDelegate sharedInstance] application:application
                                                                  openURL:url
                                                        sourceApplication:options[UIApplicationOpenURLOptionsSourceApplicationKey]
                                                               annotation:options[UIApplicationOpenURLOptionsAnnotationKey]];
    return handled;
}


@end
