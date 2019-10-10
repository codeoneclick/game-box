//
//  push_notifications_provider.cpp
//  gbDemo
//
//  Created by serhii.s on 10/9/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "push_notifications_provider.h"
#include <UserNotifications/UserNotifications.h>

@interface push_notifications_provider_impl : NSObject

@property (nonatomic) BOOL m_is_enabled;

+ (push_notifications_provider_impl* )shared_instance;
- (void)authenticate;
- (void)schedule_day_notifications;
- (void)schedule_offline_notifications;

@end

@implementation push_notifications_provider_impl

+ (push_notifications_provider_impl* )shared_instance
{
    static push_notifications_provider_impl *instance = nil;
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
        
    }
    return self;
}

- (void)authenticate
{

#if defined(__IOS__)
    
    UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];
    [center requestAuthorizationWithOptions:(UNAuthorizationOptionAlert + UNAuthorizationOptionSound)
                          completionHandler:^(BOOL granted, NSError * error) {
        self.m_is_enabled = granted;
        [self schedule_day_notifications];
     }];
  
#endif
    
}

- (void)schedule_day_notifications
{
#if defined(__IOS__)
        
    UNMutableNotificationContent* content = [[UNMutableNotificationContent alloc] init];
    content.title = [NSString localizedUserNotificationStringForKey:@"Wake up!" arguments:nil];
    content.body = [NSString localizedUserNotificationStringForKey:@"Join the race to be first who completed daily tasks!"
            arguments:nil];
    content.sound = [UNNotificationSound defaultSound];
     
    NSDateComponents* date = [[NSDateComponents alloc] init];
    date.hour = 8;
    date.minute = 30;
    UNCalendarNotificationTrigger* trigger = [UNCalendarNotificationTrigger
           triggerWithDateMatchingComponents:date repeats:YES];
     
    UNNotificationRequest* request = [UNNotificationRequest
           requestWithIdentifier:@"Time8:30" content:content trigger:trigger];
    
    UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];
    [center addNotificationRequest:request withCompletionHandler:^(NSError * error) {
       if (error != nil) {
           NSLog(@"%@", error.localizedDescription);
       }
    }];
    
    content = [[UNMutableNotificationContent alloc] init];
    content.title = [NSString localizedUserNotificationStringForKey:@"Hey, Wazzup!" arguments:nil];
    content.body = [NSString localizedUserNotificationStringForKey:@"Join the race, let's increase your skill!"
            arguments:nil];
    content.sound = [UNNotificationSound defaultSound];
     
    date = [[NSDateComponents alloc] init];
    date.hour = 10;
    date.minute = 30;
    trigger = [UNCalendarNotificationTrigger
           triggerWithDateMatchingComponents:date repeats:YES];
     
    request = [UNNotificationRequest
           requestWithIdentifier:@"Time10:30" content:content trigger:trigger];
    
    center = [UNUserNotificationCenter currentNotificationCenter];
    [center addNotificationRequest:request withCompletionHandler:^(NSError * error) {
       if (error != nil) {
           NSLog(@"%@", error.localizedDescription);
       }
    }];
    
    content = [[UNMutableNotificationContent alloc] init];
    content.title = [NSString localizedUserNotificationStringForKey:@"Time to Race!" arguments:nil];
    content.body = [NSString localizedUserNotificationStringForKey:@"Join the race, and see your results in leaderboard!"
                                                         arguments:nil];
    content.sound = [UNNotificationSound defaultSound];
    
    date = [[NSDateComponents alloc] init];
    date.hour = 12;
    date.minute = 30;
    trigger = [UNCalendarNotificationTrigger
               triggerWithDateMatchingComponents:date repeats:YES];
    
    request = [UNNotificationRequest
               requestWithIdentifier:@"Time12:30" content:content trigger:trigger];
    
    center = [UNUserNotificationCenter currentNotificationCenter];
    [center addNotificationRequest:request withCompletionHandler:^(NSError * error) {
         if (error != nil) {
            NSLog(@"%@", error.localizedDescription);
         }
    }];
    
    content = [[UNMutableNotificationContent alloc] init];
    content.title = [NSString localizedUserNotificationStringForKey:@"OMG, did you see it!" arguments:nil];
    content.body = [NSString localizedUserNotificationStringForKey:@"Join the race to unlock and customize your new car!"
                                                         arguments:nil];
    content.sound = [UNNotificationSound defaultSound];
    
    date = [[NSDateComponents alloc] init];
    date.hour = 16;
    date.minute = 30;
    trigger = [UNCalendarNotificationTrigger
               triggerWithDateMatchingComponents:date repeats:YES];
    
    request = [UNNotificationRequest
               requestWithIdentifier:@"Time16:30" content:content trigger:trigger];
    
    center = [UNUserNotificationCenter currentNotificationCenter];
    [center addNotificationRequest:request withCompletionHandler:^(NSError * error) {
         if (error != nil) {
            NSLog(@"%@", error.localizedDescription);
         }
    }];
    
#endif
}

- (void)schedule_offline_notifications
{
#if defined(__IOS__)
        
    UNMutableNotificationContent* content = [[UNMutableNotificationContent alloc] init];
    content.title = [NSString localizedUserNotificationStringForKey:@"Don't miss new Race!" arguments:nil];
    content.body = [NSString localizedUserNotificationStringForKey:@"Other racers waiting for you!"
                                                         arguments:nil];
    content.sound = [UNNotificationSound defaultSound];

    UNTimeIntervalNotificationTrigger* trigger = [UNTimeIntervalNotificationTrigger
                                              triggerWithTimeInterval:15 * 60 repeats:NO];
    
    UNNotificationRequest* request = [UNNotificationRequest
                                      requestWithIdentifier:@"TimeOFFLINE5Mimutes" content:content trigger:trigger];
    
    UNUserNotificationCenter* center = [UNUserNotificationCenter currentNotificationCenter];
    [center addNotificationRequest:request withCompletionHandler:^(NSError * error) {
         if (error != nil) {
            NSLog(@"%@", error.localizedDescription);
         }
    }];
    
    content = [[UNMutableNotificationContent alloc] init];
    content.title = [NSString localizedUserNotificationStringForKey:@"Don't miss new Race!" arguments:nil];
    content.body = [NSString localizedUserNotificationStringForKey:@"Other racers waiting for you!"
                                                         arguments:nil];
    content.sound = [UNNotificationSound defaultSound];
    
    trigger = [UNTimeIntervalNotificationTrigger
               triggerWithTimeInterval:60 * 60 repeats:NO];
    
    request = [UNNotificationRequest
               requestWithIdentifier:@"TimeOFFLINE60Mimutes" content:content trigger:trigger];
    
    center = [UNUserNotificationCenter currentNotificationCenter];
    [center addNotificationRequest:request withCompletionHandler:^(NSError * error) {
         if (error != nil) {
            NSLog(@"%@", error.localizedDescription);
         }
    }];
      
    #endif
}

@end

namespace game
{
    std::shared_ptr<push_notifications_provider> push_notifications_provider::m_instance = nullptr;
    
    push_notifications_provider::push_notifications_provider()
    {
        [push_notifications_provider_impl shared_instance];
    }
    
    push_notifications_provider::~push_notifications_provider()
    {
        
    }
    
    std::shared_ptr<push_notifications_provider> push_notifications_provider::shared_instance()
    {
        if(!m_instance)
        {
            m_instance = std::make_shared<push_notifications_provider>();
        }
        return m_instance;
    }
    
    void push_notifications_provider::authenticate()
    {
        [[push_notifications_provider_impl shared_instance] authenticate];
    }

    void push_notifications_provider::schedule_day_notifications()
    {
         [[push_notifications_provider_impl shared_instance] schedule_day_notifications];
    }

    void push_notifications_provider::schedule_offline_notifications()
    {
        [[push_notifications_provider_impl shared_instance] schedule_offline_notifications];
    }
}
