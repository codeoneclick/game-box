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

static NSString* k_gad_app_id = @"ca-app-pub-1153408705768772~1834702252";
static NSString* k_fuel_reward_video_id = @"ca-app-pub-1153408705768772/7403681567";
static NSString* k_free_restart_reward_video_id = @"";
static NSString* k_banner_ad_id = @"ca-app-pub-1153408705768772/4886072660";

@interface advertisement_provider_impl : NSObject<GADRewardBasedVideoAdDelegate, GADBannerViewDelegate>

@property(nonatomic, unsafe_unretained) UINavigationController* m_root_navigation_controller;
@property(nonatomic, unsafe_unretained) UIView* m_root_view;
@property(nonatomic, strong) GADBannerView *m_banner_view;

+ (advertisement_provider_impl* )shared_instance;
- (void)assign_root_navigation_controller:(UINavigationController*) root_navigation_controller;
- (void)play_rewarded_video;
- (void)show_banner;

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
        [GADMobileAds configureWithApplicationID:k_gad_app_id];
        
        [GADRewardBasedVideoAd sharedInstance].delegate = self;
        GADRequest* request = [GADRequest request];
        [request setTestDevices:@[@"d64c1849da6d3b14d4c8a034f61540d1964e3ee8"]];
        [[GADRewardBasedVideoAd sharedInstance] loadRequest:request
                                               withAdUnitID:k_fuel_reward_video_id];
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

- (void)play_rewarded_video
{
    if ([[GADRewardBasedVideoAd sharedInstance] isReady])
    {
        [[GADRewardBasedVideoAd sharedInstance] presentFromRootViewController:self->_m_root_navigation_controller];
    }
}

- (void)show_banner
{
    f32 screen_width = [[UIScreen mainScreen] bounds].size.width;
    f32 screen_height = [[UIScreen mainScreen] bounds].size.height;
    self->_m_banner_view = [[GADBannerView alloc] initWithFrame:CGRectMake(screen_width * 0.5 - kGADAdSizeBanner.size.width * 0.5,
                                                                           screen_height - kGADAdSizeBanner.size.height,
                                                                           kGADAdSizeBanner.size.width,
                                                                           kGADAdSizeBanner.size.height)];
    self->_m_banner_view.adUnitID = k_banner_ad_id;
    self->_m_banner_view.rootViewController = self->_m_root_navigation_controller;
    self->_m_banner_view.delegate = self;
    GADRequest* request = [GADRequest request];
    [request setTestDevices:@[@"d64c1849da6d3b14d4c8a034f61540d1964e3ee8"]];
    [self->_m_banner_view loadRequest:request];
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
    GADRequest* request = [GADRequest request];
    [request setTestDevices:@[@"d64c1849da6d3b14d4c8a034f61540d1964e3ee8"]];
    [[GADRewardBasedVideoAd sharedInstance] loadRequest:request
                                           withAdUnitID:k_fuel_reward_video_id];
}

- (void)rewardBasedVideoAdWillLeaveApplication:(GADRewardBasedVideoAd *)rewardBasedVideoAd
{
    NSLog(@"Reward based video ad will leave application.");
}

- (void)rewardBasedVideoAd:(GADRewardBasedVideoAd *)rewardBasedVideoAd didFailToLoadWithError:(NSError *)error
{
    NSLog(@"Reward based video ad failed to load with error: %@.", error);
}

- (void)adViewDidReceiveAd:(GADBannerView *)banner_view {
    NSLog(@"adViewDidReceiveAd");
    
    [self->_m_root_view addSubview:banner_view];
    /*[self->_m_root_view addConstraints:@[[NSLayoutConstraint constraintWithItem:self->_m_banner_view
                                                                      attribute:NSLayoutAttributeBottom
                                                                      relatedBy:NSLayoutRelationEqual
                                                                         toItem:self->_m_root_view
                                                                      attribute:NSLayoutAttributeTop
                                                                     multiplier:1
                                                                       constant:0],
                                         [NSLayoutConstraint constraintWithItem:self->_m_banner_view
                                                                      attribute:NSLayoutAttributeCenterX
                                                                      relatedBy:NSLayoutRelationEqual
                                                                         toItem:self->_m_root_view
                                                                      attribute:NSLayoutAttributeCenterX
                                                                     multiplier:1
                                                                       constant:0]]];*/
}

- (void)adView:(GADBannerView *)adView didFailToReceiveAdWithError:(GADRequestError *)error {
    NSLog(@"adView:didFailToReceiveAdWithError: %@", [error localizedDescription]);
}


- (void)adViewWillPresentScreen:(GADBannerView *)adView {
    NSLog(@"adViewWillPresentScreen");
}

- (void)adViewWillDismissScreen:(GADBannerView *)adView {
    NSLog(@"adViewWillDismissScreen");
}

- (void)adViewDidDismissScreen:(GADBannerView *)adView {
    NSLog(@"adViewDidDismissScreen");
}

- (void)adViewWillLeaveApplication:(GADBannerView *)adView {
    NSLog(@"adViewWillLeaveApplication");
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
    
    void advertisement_provider::assign_root_view(void* view)
    {
        
#if defined(__IOS__)
        
        [[advertisement_provider_impl shared_instance] assign_root_view:(__bridge UIView *)view];
        
#endif
        
    }
    
    void advertisement_provider::play_rewarded_video()
    {
        
#if defined(__IOS__)
        
        [[advertisement_provider_impl shared_instance] play_rewarded_video];
        
#endif
        
    }
    
    void advertisement_provider::show_banner()
    {
        
#if defined(__IOS__)
        
        [[advertisement_provider_impl shared_instance] show_banner];
        
#endif
        
    }
}
