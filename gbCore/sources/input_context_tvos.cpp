//
//  input_context_tvos.cpp
//  gbCore
//
//  Created by serhii serhiiv on 4/5/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "input_context.h"

#if defined(__TVOS__) && USED_GRAPHICS_API != NO_GRAPHICS_API

#include "window_impl.h"

#include <Foundation/Foundation.h>
#include <UIKit/UIKit.h>

@interface input_hwnd : UIView

@property(nonatomic, unsafe_unretained) gb::input_context* m_context;
@property(nonatomic, strong) NSMutableSet* m_unique_touches;
@property(nonatomic, strong) NSMutableDictionary<NSNumber*, UITouch*>* m_touches_indexes;

@end

@implementation input_hwnd

- (instancetype)init
{
    self = [super init];
    self.m_unique_touches = [NSMutableSet new];
    self.m_touches_indexes = [NSMutableDictionary new];
    return self;
}

- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
    assert(self.m_context != nullptr);
    for (UITouch* touch in [event allTouches])
    {
        if(![self.m_unique_touches containsObject:touch])
        {
            CGPoint point = [touch locationInView:self];
            glm::ivec2 current_touch_point = glm::ivec2(point.x, point.y);
            ui32 index = static_cast<ui32>([self.m_unique_touches count]);
            self.m_context->gr_pressed(current_touch_point, self.m_context->get_touch_area_size(), gb::e_input_source_mouse_left, index);
            self.m_context->set_previous_touch_point(current_touch_point);
            [self.m_unique_touches addObject:touch];
            [self.m_touches_indexes setObject:touch forKey:@(index)];
        }
    }
}

- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
    assert(self.m_context != nullptr);
    for (UITouch* touch in touches)
    {
        NSSet* keys = [self.m_touches_indexes keysOfEntriesPassingTest:^BOOL(NSNumber* key, UITouch *aTouch, BOOL *stop) {
            return [aTouch isEqual:touch];
        }];
        
        CGPoint point = [touch locationInView:self];
        glm::ivec2 current_touch_point = glm::ivec2(point.x, point.y);
        glm::ivec2 delta = current_touch_point - self.m_context->get_previous_touch_point();
        self.m_context->gr_dragged(current_touch_point,
                                   self.m_context->get_touch_area_size(),
                                   delta, gb::e_input_source_mouse_left,
                                   static_cast<ui32>([[keys anyObject] unsignedIntegerValue]));
        self.m_context->set_previous_touch_point(current_touch_point);
    }
}

- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
    assert(self.m_context != nullptr);
    for (UITouch* touch in touches)
    {
        NSSet* keys = [self.m_touches_indexes keysOfEntriesPassingTest:^BOOL(NSNumber* key, UITouch *aTouch, BOOL *stop) {
            return [aTouch isEqual:touch];
        }];
        
        CGPoint point = [touch locationInView:self];
        glm::ivec2 current_touch_point = glm::ivec2(point.x, point.y);
        self.m_context->gr_released(current_touch_point,
                                    self.m_context->get_touch_area_size(),
                                    gb::e_input_source_mouse_left,
                                    static_cast<ui32>([[keys anyObject] unsignedIntegerValue]));
        self.m_context->set_previous_touch_point(current_touch_point);
        
        [self.m_unique_touches removeObject:touch];
        
        NSNumber *currentKey = [keys anyObject];
        if(currentKey)
        {
            [self.m_touches_indexes removeObjectForKey:[keys anyObject]];
        }
    }
}

- (void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event
{
    assert(self.m_context != nullptr);
    for (UITouch* touch in touches)
    {
        NSSet* keys = [self.m_touches_indexes keysOfEntriesPassingTest:^BOOL(NSNumber* key, UITouch *aTouch, BOOL *stop) {
            return [aTouch isEqual:touch];
        }];
        
        CGPoint point = [touch locationInView:self];
        glm::ivec2 current_touch_point = glm::ivec2(point.x, point.y);
        self.m_context->gr_released(current_touch_point,
                                    self.m_context->get_touch_area_size(),
                                    gb::e_input_source_mouse_left,
                                    static_cast<ui32>([[keys anyObject] unsignedIntegerValue]));
        self.m_context->set_previous_touch_point(current_touch_point);
        
        [self.m_unique_touches removeObject:touch];
        NSNumber *currentKey = [keys anyObject];
        if(currentKey)
        {
            [self.m_touches_indexes removeObjectForKey:[keys anyObject]];
        }
    }
}

@end

namespace gb
{
    class input_context_tvos : public input_context
    {
    private:
        
    protected:
        
    public:
        
        input_context_tvos(const std::shared_ptr<window_impl>& window);
        ~input_context_tvos();
    };
    
    std::shared_ptr<input_context> create_input_context_tvos(const std::shared_ptr<window_impl>& window)
    {
        return std::make_shared<input_context_tvos>(window);
    };
    
    input_context_tvos::input_context_tvos(const std::shared_ptr<window_impl>& window)
    {
        UIView* view = (__bridge UIView*)window->get_hwnd();
        
        input_hwnd* input_view = [[input_hwnd alloc] init];
        input_view.m_context = this;
        input_view.frame = CGRectMake(0.f, 0.f,
                                      view.frame.size.width, view.frame.size.height);
        [view addSubview:input_view];
    }
    
    input_context_tvos::~input_context_tvos()
    {
        
    }
}
#endif


