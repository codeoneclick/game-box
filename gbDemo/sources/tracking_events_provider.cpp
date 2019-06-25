//
//  tracking_events_provider.cpp
//  gbDemo
//
//  Created by serhii.s on 6/20/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "tracking_events_provider.h"


#if defined(__IOS__)

#import <UIKit/UIKit.h>
#import <Firebase.h>

@interface tracking_events_provider_impl : NSObject

+ (tracking_events_provider_impl* )shared_instance;
- (void)on_level_enter:(i32) level_id;
- (void)on_level_finished:(i32) level_id first_place_achievement:(i32) param_1 low_damage_achievement:(i32) param_2 good_drift_achievement:(i32) param_3;
- (void)on_car_damaged:(i32) level_id;
- (void)on_rank_updated:(i32) rank;
- (void)on_car_selected:(i32) car_id;

@end

@implementation tracking_events_provider_impl

+ (tracking_events_provider_impl* )shared_instance
{
    static tracking_events_provider_impl *instance = nil;
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
}

- (void)on_level_finished:(i32) level_id first_place_achievement:(i32) param_1 low_damage_achievement:(i32) param_2 good_drift_achievement:(i32) param_3
{
    [FIRAnalytics logEventWithName:@"on_level_finished" parameters:@{@"level_id": @(level_id),
                                                                     @"first_place_achievement": @(param_1),
                                                                     @"low_damage_achievement": @(param_2),
                                                                     @"good_drift_achievement": @(param_3)}];
}

- (void)on_car_damaged:(i32) level_id
{
    [FIRAnalytics logEventWithName:@"on_car_damaged" parameters:@{@"level_id": @(level_id)}];
}


- (void)on_rank_updated:(i32) rank
{
    [FIRAnalytics logEventWithName:@"on_rank_updated" parameters:@{@"rank": @(rank)}];
}

- (void)on_car_selected:(i32) car_id
{
    [FIRAnalytics logEventWithName:@"on_car_selected" parameters:@{@"car_id": @(car_id)}];
}

@end

#endif

namespace game
{
    std::shared_ptr<tracking_events_provider> tracking_events_provider::m_instance = nullptr;
    
    tracking_events_provider::tracking_events_provider()
    {
        
#if defined(__IOS__)
        

        
#endif
        
    }
    
    tracking_events_provider::~tracking_events_provider()
    {
        
    }
    
    std::shared_ptr<tracking_events_provider> tracking_events_provider::shared_instance()
    {
        if(!m_instance)
        {
            m_instance = std::make_shared<tracking_events_provider>();
        }
        return m_instance;
    }
    
    void tracking_events_provider::on_level_enter(i32 level_id)
    {
        
#if defined(__IOS__)
        
        [[tracking_events_provider_impl shared_instance] on_level_enter: level_id];
        
#endif
        
    }
    
    void tracking_events_provider::on_level_finished(i32 level_id, i32 first_place_achievement, i32 low_damage_achievement, i32 good_drift_achievement)
    {
        
#if defined(__IOS__)
        
        [[tracking_events_provider_impl shared_instance] on_level_finished:level_id
                                                   first_place_achievement:first_place_achievement
                                                    low_damage_achievement:low_damage_achievement
                                                    good_drift_achievement:good_drift_achievement];
        
#endif
        
    }
    
    void tracking_events_provider::on_car_damaged(i32 level_id)
    {
        
#if defined(__IOS__)
        
        [[tracking_events_provider_impl shared_instance] on_car_damaged:level_id];
        
#endif
        
    }
    
    void tracking_events_provider::on_rank_updated(i32 rank)
    {
        
#if defined(__IOS__)
        
         [[tracking_events_provider_impl shared_instance] on_rank_updated: rank];
        
#endif
        
    }
    
    void tracking_events_provider::on_car_selected(i32 car_id)
    {
        
#if defined(__IOS__)
        
         [[tracking_events_provider_impl shared_instance] on_car_selected: car_id];
        
#endif
        
    }
}
