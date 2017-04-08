//
//  game_loop_tvos.cpp
//  gbCore
//
//  Created by serhii serhiiv on 4/5/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//
#if defined(__TVOS__)

#include "game_loop.h"
#include <Foundation/Foundation.h>
#include <UIKit/UIKit.h>

@interface game_loop_tvos : NSObject

@property(nonatomic, unsafe_unretained) gb::game_loop* m_game_loop;

+ (game_loop_tvos* )shared_instance;

- (void)add_listener:(const std::shared_ptr<gb::i_game_loop>&)listener;
- (void)remove_listener:(const std::shared_ptr<gb::i_game_loop>&)listener;

@end

@implementation game_loop_tvos

+ (game_loop_tvos* )shared_instance
{
    static game_loop_tvos *instance = nil;
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
        pthread_setname_np("gb.core.main");
        CADisplayLink* main_loop = [CADisplayLink displayLinkWithTarget:self selector:@selector(on_update:)];
        [main_loop addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    }
    return self;
}

- (void)add_listener:(const std::shared_ptr<gb::i_game_loop>&)listener;
{
    assert(self.m_game_loop != nullptr);
    self.m_game_loop->add_listener(listener);
}

- (void)remove_listener:(const std::shared_ptr<gb::i_game_loop>&)listener;
{
    assert(self.m_game_loop != nullptr);
    self.m_game_loop->remove_listener(listener);
}

- (void)on_update:(CADisplayLink*)display_link
{
    assert(self.m_game_loop != nullptr);
    self.m_game_loop->on_update();
}

@end

namespace gb
{
    void add_listener_to_game_loop(const std::shared_ptr<i_game_loop>& listener)
    {
        [[game_loop_tvos shared_instance] add_listener:listener];
    }
    void remove_listener_from_game_loop(const std::shared_ptr<i_game_loop>& listener)
    {
        [[game_loop_tvos shared_instance] remove_listener:listener];
    }
}
#endif

