//
//  game_center_provier.cpp
//  gbDemo
//
//  Created by serhii.s on 7/31/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "game_center_provier.h"
#include <GameKit/GameKit.h>

static NSString* k_drifters_leaderboard_id = @"drifters";

@interface game_center_provier_impl : NSObject<GKGameCenterControllerDelegate>

#if defined(__IOS__)

@property (nonatomic, strong) UIViewController* m_presentation_controller;

#else

@property (nonatomic, strong) NSViewController* m_presentation_controller;

#endif

@property (nonatomic) NSString* m_leaderboard_id;

+ (game_center_provier_impl* )shared_instance;
- (void)authenticate;

- (void)open_drifters_leaderboard;
- (void)report_drifring_score:(i32)seconds;

@end

@implementation game_center_provier_impl

+ (game_center_provier_impl* )shared_instance
{
    static game_center_provier_impl *instance = nil;
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
        
#if defined(__IOS__)
        
        
        self.m_presentation_controller = [[[[UIApplication sharedApplication] delegate] window] rootViewController];
        
#else
        
        self.m_presentation_controller = [[[NSApplication sharedApplication] mainWindow] contentViewController];
        
#endif
        
        self.m_leaderboard_id = k_drifters_leaderboard_id;
    }
    return self;
}

- (void)authenticate
{
    GKLocalPlayer *local_player = [GKLocalPlayer localPlayer];
   
    
#if defined(__IOS__)
    
    [local_player setAuthenticateHandler:^(UIViewController* view_controller, NSError *error) {
        
        if (view_controller != nil)
        {
            [self.m_presentation_controller presentViewController:view_controller animated:YES completion:nil];
        }
        else if ([GKLocalPlayer localPlayer].authenticated)
        {
            [[GKLocalPlayer localPlayer] loadDefaultLeaderboardIdentifierWithCompletionHandler:^(NSString *leaderboard_id, NSError *error) {
                
                if (error != nil)
                {
                    NSLog(@"%@", [error localizedDescription]);
                }
                else
                {
                    self.m_leaderboard_id = leaderboard_id;
                }
            }];
        }
        else if (error)
        {
            NSLog(@"Game Center authentication error: %@", error);
        }
    }];
    
#else
    
    [local_player setAuthenticateHandler:^(NSViewController* view_controller, NSError *error) {
        
        if (view_controller != nil)
        {
            [[GKDialogController sharedDialogController] presentViewController:view_controller];
        }
        else if ([GKLocalPlayer localPlayer].authenticated)
        {
            [[GKLocalPlayer localPlayer] loadDefaultLeaderboardIdentifierWithCompletionHandler:^(NSString *leaderboard_id, NSError *error) {
                    
                if (error != nil)
                {
                    NSLog(@"%@", [error localizedDescription]);
                }
                else
                {
                    self.m_leaderboard_id = leaderboard_id;
                }
            }];
        }
        else if (error)
        {
            NSLog(@"Game Center authentication error: %@", error);
        }
    }];
    
#endif
    
}

- (void)open_drifters_leaderboard
{
    GKGameCenterViewController *gc_view_controller = [[GKGameCenterViewController alloc] init];
    gc_view_controller.gameCenterDelegate = self;
    gc_view_controller.viewState = GKGameCenterViewControllerStateLeaderboards;
    gc_view_controller.leaderboardIdentifier = self.m_leaderboard_id;
    
#if defined(__IOS__)
    
    
    [self.m_presentation_controller presentViewController:gc_view_controller animated:YES completion:nil];
    
#else
    
    [[GKDialogController sharedDialogController] presentViewController:gc_view_controller];
    
#endif
    
}

- (void)gameCenterViewControllerDidFinish:(GKGameCenterViewController *)game_center_view_controller
{
    
#if defined(__IOS__)
    
    [game_center_view_controller dismissViewControllerAnimated:YES completion:nil];
    
#else
    
     [[GKDialogController sharedDialogController] dismiss:game_center_view_controller];
    
#endif
   
}

- (void)report_drifring_score:(i32)seconds
{
    GKScore *score = [[GKScore alloc] initWithLeaderboardIdentifier:self.m_leaderboard_id];
    score.value = seconds;
    score.context = 0;
    
    [GKScore reportScores:@[score] withCompletionHandler:^(NSError *error) {
        if (!error)
        {
            NSLog(@"score reported successfully!");
        }
        else
        {
            NSLog(@"unable to report score");
        }
    }];
}

@end

namespace game
{
    std::shared_ptr<game_center_provier> game_center_provier::m_instance = nullptr;
    
    game_center_provier::game_center_provier()
    {
        [game_center_provier_impl shared_instance];
    }
    
    game_center_provier::~game_center_provier()
    {
        
    }
    
    std::shared_ptr<game_center_provier> game_center_provier::shared_instance()
    {
        if(!m_instance)
        {
            m_instance = std::make_shared<game_center_provier>();
        }
        return m_instance;
    }
    
    void game_center_provier::authenticate()
    {
        [[game_center_provier_impl shared_instance] authenticate];
    }
    
    void game_center_provier::open_drifters_leaderboard()
    {
        [[game_center_provier_impl shared_instance] open_drifters_leaderboard];
    }
    
    void game_center_provier::report_drifring_score(i32 seconds)
    {
        [[game_center_provier_impl shared_instance] report_drifring_score:seconds];
    }
}
