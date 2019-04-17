//
//  advertisement_provider.cpp
//  gbDemo
//
//  Created by serhii.s on 4/17/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "advertisement_provider.h"

#if defined(__IOS__)

#import <UIKit/UIKit.h>
#import <Firebase.h>

@interface advertisement_provider_impl : NSObject<GADRewardBasedVideoAdDelegate>

@property(nonatomic, unsafe_unretained) UINavigationController* m_root_navigation_controller;

+ (advertisement_provider_impl* )shared_instance;
- (void)assign_root_navigation_controller:(UINavigationController*) root_navigation_controller;
- (void)play_rewarded_video;

@end

@implementation advertisement_provider_impl

+ (advertisement_provider_impl* )shared_instance
{
    static advertisement_provider_impl *instance = nil;
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
        [GADMobileAds configureWithApplicationID:@"ca-app-pub-1153408705768772~1834702252"];
        
        [GADRewardBasedVideoAd sharedInstance].delegate = self;
        GADRequest* request = [GADRequest request];
        [[GADRewardBasedVideoAd sharedInstance] loadRequest:request
                                               withAdUnitID:@"ca-app-pub-3940256099942544/1712485313"];
    }
    return self;
}

- (void)assign_root_navigation_controller:(UINavigationController*) root_navigation_controller
{
    self->_m_root_navigation_controller = root_navigation_controller;
}

- (void)play_rewarded_video
{
    if ([[GADRewardBasedVideoAd sharedInstance] isReady])
    {
        [[GADRewardBasedVideoAd sharedInstance] presentFromRootViewController:self->_m_root_navigation_controller];
    }
}

- (void)rewardBasedVideoAd:(GADRewardBasedVideoAd *)rewardBasedVideoAd didRewardUserWithReward:(GADAdReward *)reward
{
    NSString *rewardMessage = [NSString stringWithFormat:@"Reward received with currency %@ , amount %lf", reward.type, [reward.amount doubleValue]];
    NSLog(@"%@", rewardMessage);
}

- (void)rewardBasedVideoAdDidReceiveAd:(GADRewardBasedVideoAd *)rewardBasedVideoAd
{
    NSLog(@"Reward based video ad is received.");
}

- (void)rewardBasedVideoAdDidOpen:(GADRewardBasedVideoAd *)rewardBasedVideoAd
{
    NSLog(@"Opened reward based video ad.");
}

- (void)rewardBasedVideoAdDidStartPlaying:(GADRewardBasedVideoAd *)rewardBasedVideoAd
{
    NSLog(@"Reward based video ad started playing.");
}

- (void)rewardBasedVideoAdDidCompletePlaying:(GADRewardBasedVideoAd *)rewardBasedVideoAd
{
    NSLog(@"Reward based video ad has completed.");
}

- (void)rewardBasedVideoAdDidClose:(GADRewardBasedVideoAd *)rewardBasedVideoAd
{
    NSLog(@"Reward based video ad is closed.");
    [[GADRewardBasedVideoAd sharedInstance] loadRequest:[GADRequest request] withAdUnitID:@"ca-app-pub-3940256099942544/1712485313"];
}

- (void)rewardBasedVideoAdWillLeaveApplication:(GADRewardBasedVideoAd *)rewardBasedVideoAd
{
    NSLog(@"Reward based video ad will leave application.");
}

- (void)rewardBasedVideoAd:(GADRewardBasedVideoAd *)rewardBasedVideoAd didFailToLoadWithError:(NSError *)error
{
    NSLog(@"Reward based video ad failed to load with error: %@.", error);
}

@end

#endif

namespace game
{
    std::shared_ptr<advertisement_provider> advertisement_provider::m_instance = nullptr;
        
    advertisement_provider::advertisement_provider()
    {
        
#if defined(__IOS__)
        
         [advertisement_provider_impl shared_instance];
        
#endif
        
    }
    
    advertisement_provider::~advertisement_provider()
    {
        
    }
        
    std::shared_ptr<advertisement_provider> advertisement_provider::shared_instance()
    {
        if(!m_instance)
        {
            m_instance = std::make_shared<advertisement_provider>();
        }
        return m_instance;
    }
    
    void advertisement_provider::assign_root_navigation_controller(void* root_navigation_controller)
    {
        
#if defined(__IOS__)
    
    [[advertisement_provider_impl shared_instance] assign_root_navigation_controller:(__bridge UINavigationController *)root_navigation_controller];
    
#endif
    
    }
    
    void advertisement_provider::play_rewarded_video()
    {
        
#if defined(__IOS__)
        
        [[advertisement_provider_impl shared_instance] play_rewarded_video];
        
#endif
        
    }
}
