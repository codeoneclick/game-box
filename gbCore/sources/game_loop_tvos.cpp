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

#if USED_GRAPHICS_API == METAL_API

#include <MetalKit/MetalKit.h>

@interface game_loop_tvos : NSObject<MTKViewDelegate>

#else

@interface game_loop_tvos : NSObject

#endif

@property(nonatomic, unsafe_unretained) gb::game_loop* m_game_loop;

+ (game_loop_tvos* )shared_instance;

#if USED_GRAPHICS_API == METAL_API

- (void)assign_hwnd:(MTKView*)hwnd;

#endif

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
        
#if USED_GRAPHICS_API != NO_GRAPHICS_API && USED_GRAPHICS_API != METAL_API
        
        CADisplayLink* main_loop = [CADisplayLink displayLinkWithTarget:self selector:@selector(on_update:)];
        [main_loop addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        
#endif
        
    }
    return self;
}

#if USED_GRAPHICS_API == METAL_API

- (void)assign_hwnd:(MTKView*)hwnd
{
    hwnd.delegate = self;
}

#endif

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

#if USED_GRAPHICS_API == METAL_API

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size
{
    
}

- (void)drawInMTKView:(nonnull MTKView *)view
{
    @autoreleasepool
    {
        [self on_update:nil];
    }
}

#endif

@end

namespace gb
{
    void assign_hwnd_to_game_loop(const void* hwnd)
    {
        
#if USED_GRAPHICS_API == METAL_API
        
        MTKView *mtl_hwnd = (__bridge MTKView *)hwnd;
        [[game_loop_tvos shared_instance] assign_hwnd:mtl_hwnd];
        
#endif
        
    }
    
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

