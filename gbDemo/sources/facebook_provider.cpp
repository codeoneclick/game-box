//
//  facebook_provider.cpp
//  gbDemo
//
//  Created by serhii.s on 7/4/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "facebook_provider.h"

#if defined(__IOS__)

#import <UIKit/UIKit.h>
#import <FBSDKCoreKit/FBSDKCoreKit.h>
#import <FBSDKLoginKit/FBSDKLoginKit.h>

@interface facebook_provider_impl : NSObject

@property(nonatomic, unsafe_unretained) UINavigationController* m_root_navigation_controller;
@property(nonatomic, unsafe_unretained) UIView* m_root_view;
@property(nonatomic, unsafe_unretained)FBSDKProfile* m_profile;

+ (facebook_provider_impl* )shared_instance;
- (void)assign_root_navigation_controller:(UINavigationController*) root_navigation_controller;
- (void)login;
- (void)logout;

@end

@implementation facebook_provider_impl

+ (facebook_provider_impl* )shared_instance
{
    static facebook_provider_impl *instance = nil;
    static dispatch_once_t once;
    dispatch_once(&once, ^{
        instance = [[self alloc] init];
    });
    return instance;
}

- (instancetype)init
{
    self = [super init];
    if(self)
    {
        [FBSDKProfile enableUpdatesOnAccessTokenChange:YES];
        [[NSNotificationCenter defaultCenter] addObserverForName:FBSDKProfileDidChangeNotification
                                                          object:nil
                                                           queue:[NSOperationQueue mainQueue]
                                                      usingBlock:^(NSNotification *notification) {
             if ([FBSDKProfile currentProfile]) {
                self.m_profile = [FBSDKProfile currentProfile];
             }
         }];
    }
    return self;
}

- (void)assign_root_navigation_controller:(UINavigationController*) root_navigation_controller
{
    self->_m_root_navigation_controller = root_navigation_controller;
}

- (void)assign_root_view:(UIView*) view
{
    self->_m_root_view = view;
}

- (void)login
{
    FBSDKLoginManager *login = [[FBSDKLoginManager alloc] init];
    [login logInWithReadPermissions: @[@"public_profile"]
                 fromViewController: [self->_m_root_navigation_controller visibleViewController]
                            handler:^(FBSDKLoginManagerLoginResult *result, NSError *error) {
                                if (error) {
                                    NSLog(@"facebook login error");
                                    auto callback = game::facebook_provider::shared_instance()->get_on_login();
                                    if (callback)
                                    {
                                        callback(true);
                                    }
                                } else if (result.isCancelled) {
                                    NSLog(@"facebook login cancelled");
                                    auto callback = game::facebook_provider::shared_instance()->get_on_login();
                                    if (callback)
                                    {
                                        callback(true);
                                    }
                                } else {
                                    NSLog(@"facebook logged in");
                                    auto callback = game::facebook_provider::shared_instance()->get_on_login();
                                    if (callback)
                                    {
                                        callback(false);
                                    }
                                }
                            }];
}

- (void)logout
{

}

@end

#endif

namespace game
{
    std::shared_ptr<facebook_provider> facebook_provider::m_instance = nullptr;
    
    facebook_provider::facebook_provider()
    {
        
#if defined(__IOS__)
        
        [facebook_provider_impl shared_instance];
        
#endif
        
    }
    
    facebook_provider::~facebook_provider()
    {
        
    }
    
    std::shared_ptr<facebook_provider> facebook_provider::shared_instance()
    {
        if(!m_instance)
        {
            m_instance = std::make_shared<facebook_provider>();
        }
        return m_instance;
    }
    
    void facebook_provider::assign_root_navigation_controller(void* root_navigation_controller)
    {
        
#if defined(__IOS__)
        
        [[facebook_provider_impl shared_instance] assign_root_navigation_controller:(__bridge UINavigationController *)root_navigation_controller];
        
#endif
        
    }
    
    void facebook_provider::assign_root_view(void* view)
    {
        
#if defined(__IOS__)
        
        [[facebook_provider_impl shared_instance] assign_root_view:(__bridge UIView *)view];
        
#endif
        
    }
    
    void facebook_provider::login()
    {
        
#if defined(__IOS__)
        
        [[facebook_provider_impl shared_instance] login];
        
#endif
        
    }
    
    void facebook_provider::logout()
    {
        
#if defined(__IOS__)
        
        [[facebook_provider_impl shared_instance] logout];
        
#endif
        
    }
    
    void facebook_provider::set_on_login(const std::function<void(bool is_error)>& callback)
    {
        m_on_login = callback;
    }
    
    const std::function<void(bool is_error)> facebook_provider::get_on_login() const
    {
        return m_on_login;
    }
    
    void facebook_provider::set_on_logout(const std::function<void(bool is_error)>& callback)
    {
        m_on_logout = callback;
    }
    
    const std::function<void(bool is_error)> facebook_provider::get_on_logout() const
    {
        return m_on_logout;
    }
}
