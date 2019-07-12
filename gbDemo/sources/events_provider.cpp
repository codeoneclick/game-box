//
//  tracking_events_provider.cpp
//  gbDemo
//
//  Created by serhii.s on 6/20/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "events_provider.h"

#if defined(__IOS__)

#import <Firebase.h>
#import <FBSDKCoreKit/FBSDKCoreKit.h>

@interface events_provider_impl : NSObject

+ (events_provider_impl* )shared_instance;
- (void)on_level_enter:(i32) level_id;
- (void)on_level_finished:(i32) level_id first_place_achievement:(i32) param_1 low_damage_achievement:(i32) param_2 good_drift_achievement:(i32) param_3 retries_count:(i32) param_4;
- (void)on_car_damaged:(i32) level_id retries_count:(i32) retries_count;
- (void)on_rank_updated:(i32) rank;
- (void)on_car_selected:(i32) car_id;

@end

@implementation events_provider_impl

+ (events_provider_impl* )shared_instance
{
    static events_provider_impl *instance = nil;
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

- (void)on_level_enter:(i32) level_id
{
    [FIRAnalytics logEventWithName:@"on_level_enter" parameters:@{@"level_id": @(level_id)}];
    
    [FBSDKAppEvents logEvent:@"on_level_enter" parameters:@{@"level_id": @(level_id)}];
}

- (void)on_level_finished:(i32) level_id first_place_achievement:(i32) param_1 low_damage_achievement:(i32) param_2 good_drift_achievement:(i32) param_3 retries_count:(i32)param_4
{
    [FIRAnalytics logEventWithName:@"on_level_finished" parameters:@{@"level_id": @(level_id),
                                                                     @"first_place_achievement": @(param_1),
                                                                     @"low_damage_achievement": @(param_2),
                                                                     @"good_drift_achievement": @(param_3),
                                                                     @"retries_count": @(param_4)}];
    
    [FBSDKAppEvents logEvent:@"on_level_finished" parameters:@{@"level_id": @(level_id),
                                                               @"first_place_achievement": @(param_1),
                                                               @"low_damage_achievement": @(param_2),
                                                               @"good_drift_achievement": @(param_3),
                                                               @"retries_count": @(param_4)}];
}

- (void)on_car_damaged:(i32) level_id retries_count:(i32)retries_count
{
    [FIRAnalytics logEventWithName:@"on_car_damaged" parameters:@{@"level_id": @(level_id), @"retries_count":@(retries_count)}];
    
    [FBSDKAppEvents logEvent:@"on_car_damaged" parameters:@{@"level_id": @(level_id), @"retries_count":@(retries_count)}];
}


- (void)on_rank_updated:(i32) rank
{
    [FIRAnalytics logEventWithName:@"on_rank_updated" parameters:@{@"rank": @(rank)}];
    
    [FBSDKAppEvents logEvent:@"on_rank_updated" parameters:@{@"rank": @(rank)}];
}

- (void)on_car_selected:(i32) car_id
{
    [FIRAnalytics logEventWithName:@"on_car_selected" parameters:@{@"car_id": @(car_id)}];
    
    [FBSDKAppEvents logEvent:@"on_car_selected" parameters:@{@"car_id": @(car_id)}];
}

@end

#endif

namespace game
{
    std::shared_ptr<events_provider> events_provider::m_instance = nullptr;
    
    events_provider::events_provider()
    {
        
#if defined(__IOS__)
        
        
        
#endif
        
    }
    
    events_provider::~events_provider()
    {
        
    }
    
    std::shared_ptr<events_provider> events_provider::shared_instance()
    {
        if(!m_instance)
        {
            m_instance = std::make_shared<events_provider>();
        }
        return m_instance;
    }
    
    void events_provider::on_level_enter(i32 level_id)
    {
        
#if defined(__IOS__)
        
        [[events_provider_impl shared_instance] on_level_enter: level_id];
        
#endif
        
    }
    
    void events_provider::on_level_finished(i32 level_id, i32 first_place_achievement, i32 low_damage_achievement, i32 good_drift_achievement, i32 retries_count)
    {
        
#if defined(__IOS__)
        
        [[events_provider_impl shared_instance] on_level_finished:level_id
                                          first_place_achievement:first_place_achievement
                                           low_damage_achievement:low_damage_achievement
                                           good_drift_achievement:good_drift_achievement
                                                    retries_count:retries_count];
        
#endif
        
    }
    
    void events_provider::on_car_damaged(i32 level_id, i32 retries_count)
    {
        
#if defined(__IOS__)
        
        [[events_provider_impl shared_instance] on_car_damaged:level_id retries_count:retries_count];
        
#endif
        
    }
    
    void events_provider::on_rank_updated(i32 rank)
    {
        
#if defined(__IOS__)
        
        [[events_provider_impl shared_instance] on_rank_updated: rank];
        
#endif
        
    }
    
    void events_provider::on_car_selected(i32 car_id)
    {
        
#if defined(__IOS__)
        
        [[events_provider_impl shared_instance] on_car_selected: car_id];
        
#endif
        
    }
}
