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

static NSString* k_gad_app_id = @"ca-app-pub-1553580872284996~3793679352";
static NSString* k_ticket_reward_video_id = @"ca-app-pub-1553580872284996/9946409764";
static NSString* k_banner_ad_id = @"ca-app-pub-1553580872284996/6778935345";
static NSString* k_end_level_interstitial_video_id = @"ca-app-pub-1553580872284996/9222509122";

@interface advertisement_provider_impl : NSObject<GADRewardBasedVideoAdDelegate, GADBannerViewDelegate, GADInterstitialDelegate>

@property(nonatomic, unsafe_unretained) UINavigationController* m_root_navigation_controller;
@property(nonatomic, unsafe_unretained) UIView* m_root_view;
@property(nonatomic, strong) GADBannerView *m_banner_view;
@property(nonatomic, strong) GADInterstitial *m_interstitial;

+ (advertisement_provider_impl* )shared_instance;
- (void)assign_root_navigation_controller:(UINavigationController*) root_navigation_controller;
- (bool)play_rewarded_video;
- (bool)play_interstitial_video;
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
        [request setTestDevices:@[@"38458765cb889fcffc9a79fb5f526a42"]];
        [[GADRewardBasedVideoAd sharedInstance] loadRequest:request
                                               withAdUnitID:k_ticket_reward_video_id];
        
        self->_m_interstitial = [[GADInterstitial alloc]
                                 initWithAdUnitID:k_end_level_interstitial_video_id];
        self->_m_interstitial.delegate = self;
        request = [GADRequest request];
        [request setTestDevices:@[@"38458765cb889fcffc9a79fb5f526a42"]];
        [self->_m_interstitial loadRequest:request];
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

- (bool)play_rewarded_video
{
    bool result = false;
    if ([[GADRewardBasedVideoAd sharedInstance] isReady])
    {
        [[GADRewardBasedVideoAd sharedInstance] presentFromRootViewController:self->_m_root_navigation_controller];
        result = true;
    }
    return result;
}

- (bool)play_interstitial_video
{
    bool result = false;
    if (self->_m_interstitial.isReady)
    {
        [self->_m_interstitial presentFromRootViewController:self->_m_root_navigation_controller];
        result = true;
    }
    else
    {
        NSLog(@"interstitial ad wasn't ready");
    }
    return result;
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
    [request setTestDevices:@[@"38458765cb889fcffc9a79fb5f526a42"]];
    [self->_m_banner_view loadRequest:request];
}

- (void)rewardBasedVideoAd:(GADRewardBasedVideoAd *)rewardBasedVideoAd didRewardUserWithReward:(GADAdReward *)reward
{
    NSString *reward_message = [NSString stringWithFormat:@"Reward received with currency %@ , amount %lf", reward.type, [reward.amount doubleValue]];
    NSLog(@"%@", reward_message);
    auto callback = game::advertisement_provider::shared_instance()->get_on_reward_video_viewed();
    if (callback)
    {
        callback();
    }
    
    callback = game::advertisement_provider::shared_instance()->get_on_video_ended();
    if (callback)
    {
        callback();
    }
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
    [request setTestDevices:@[@"38458765cb889fcffc9a79fb5f526a42"]];
    [[GADRewardBasedVideoAd sharedInstance] loadRequest:request
                                           withAdUnitID:k_ticket_reward_video_id];
    
    auto callback = game::advertisement_provider::shared_instance()->get_on_video_ended();
    if (callback)
    {
        callback();
    }
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

- (void)interstitialDidDismissScreen:(GADInterstitial *)interstitial {
    self->_m_interstitial = [[GADInterstitial alloc]
                             initWithAdUnitID:k_end_level_interstitial_video_id];
    self->_m_interstitial.delegate = self;
    GADRequest *request = [GADRequest request];
    [request setTestDevices:@[@"38458765cb889fcffc9a79fb5f526a42"]];
    [self->_m_interstitial loadRequest:request];
    
    auto callback = game::advertisement_provider::shared_instance()->get_on_video_ended();
    if (callback)
    {
        callback();
    }
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
    
    bool advertisement_provider::play_rewarded_video()
    {
        
#if defined(__IOS__)
        
        return [[advertisement_provider_impl shared_instance] play_rewarded_video];
        
#endif
        
        return false;
    }
    
    bool advertisement_provider::play_interstitial_video()
    {
        
#if defined(__IOS__)
        
        return [[advertisement_provider_impl shared_instance] play_interstitial_video];
        
#endif
        
        return false;
        
    }
    
    void advertisement_provider::show_banner()
    {
        
#if defined(__IOS__)
        
        [[advertisement_provider_impl shared_instance] show_banner];
        
#endif
        
    }
    
    void advertisement_provider::set_on_reward_video_viewed(const std::function<void()>& callback)
    {
        m_on_reward_video_viewed = callback;
    }
    
    const std::function<void()> advertisement_provider::get_on_reward_video_viewed() const
    {
        return m_on_reward_video_viewed;
    }
    
    void advertisement_provider::set_on_video_ended(const std::function<void()>& callback)
    {
        m_on_video_ended = callback;
    }
    
    const std::function<void()> advertisement_provider::get_on_video_ended() const
    {
        return m_on_video_ended;
    }
}
