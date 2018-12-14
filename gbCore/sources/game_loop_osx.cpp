//
//  game_loop_osx.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/12/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "game_loop.h"

#if defined(__OSX__)

#include <Cocoa/Cocoa.h>

#define k_interval 1.0f / 60.0f

@interface game_loop_osx : NSObject

@property(nonatomic, unsafe_unretained) gb::game_loop* m_game_loop;
@property(nonatomic, strong) NSTimer* m_timer;

+ (game_loop_osx*)shared_instance;

- (void)add_listener:(const std::shared_ptr<gb::i_game_loop>&)listener;
- (void)remove_listener:(const std::shared_ptr<gb::i_game_loop>&)listener;

- (void)on_update;

@end

@implementation game_loop_osx

+ (game_loop_osx*)shared_instance
{
    static game_loop_osx *instance = nil;
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
        self.m_game_loop = new gb::game_loop();
        
#if USED_GRAPHICS_API != NO_GRAPHICS_API
        
        self.m_timer = [NSTimer scheduledTimerWithTimeInterval:k_interval target:self selector:@selector(on_update) userInfo:nil repeats:YES];
        
#endif
        
    }
    return self;
}

- (void)add_listener:(const std::shared_ptr<gb::i_game_loop>&)listener
{
    assert(self.m_game_loop);
    self.m_game_loop->add_listener(listener);
}

- (void)remove_listener:(const std::shared_ptr<gb::i_game_loop>&)listener;
{
    assert(self.m_game_loop);
    self.m_game_loop->remove_listener(listener);
}

- (void)terminate
{
#if USED_GRAPHICS_API != NO_GRAPHICS_API
    
    [self.m_timer invalidate];
    
#endif
}

- (void)on_update
{
    assert(self.m_game_loop);
    self.m_game_loop->on_update();
}

@end

namespace gb
{
    void add_listener_to_game_loop(const std::shared_ptr<i_game_loop>& listener)
    {
        [[game_loop_osx shared_instance] add_listener:listener];
    }
    void remove_listener_from_game_loop(const std::shared_ptr<i_game_loop>& listener)
    {
        [[game_loop_osx shared_instance] remove_listener:listener];
    }
    
    void terminate_game_loop()
    {
         [[game_loop_osx shared_instance] terminate];
    }
    
    void execute_runloop()
    {
        while (1)
        {
            [[game_loop_osx shared_instance] on_update];
        }
    }
}

#endif
