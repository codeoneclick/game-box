//
//  input_context_ios.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/31/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "input_context.h"

#if defined(__IOS__) && USED_GRAPHICS_API != NO_GRAPHICS_API

#include "window_impl.h"

#include <Foundation/Foundation.h>
#include <UIKit/UIKit.h>

@interface input_hwnd : UIView<UIKeyInput>

@property(nonatomic, unsafe_unretained) gb::input_context* m_context;
@property(nonatomic, strong) NSMutableSet* m_unique_touches;
@property(nonatomic, strong) NSMutableDictionary<NSNumber*, UITouch*>* m_touches_indexes;
@property(nonatomic) f32 m_scale_x;
@property(nonatomic) f32 m_scale_y;

- (void)showKeyboard;
- (void)hideKeyboard;

@end

@implementation input_hwnd
@synthesize hasText;

- (instancetype)init
{
    self = [super init];
    self.m_unique_touches = [NSMutableSet new];
    self.m_touches_indexes = [NSMutableDictionary new];
    self.m_scale_x = 1.f;
    self.m_scale_y = 1.f;
    [[NSNotificationCenter defaultCenter]addObserver:self selector:@selector(onKeyboardHide:) name:UIKeyboardWillHideNotification object:nil];
    return self;
}

- (void)insertText:(NSString *)aText
{
    if([aText length])
    {
        self.m_context->virtual_keyboard_input([aText UTF8String]);
    }
}

- (void)deleteBackward
{
    self.m_context->virtual_keyboard_backspace();
}

- (BOOL)canBecomeFirstResponder
{
    return YES;
}

- (BOOL)canResignFirstResponder
{
    return YES;
}

-(void)onKeyboardHide:(NSNotification *)aNotification
{
    self.m_context->virtual_keyboard_hidden();
}

- (void)touchesBegan:(NSSet*)aTouches withEvent:(UIEvent*)anEvent
{
    assert(self.m_context != nullptr);
    for (UITouch* touch in aTouches)
    {
        if(![self.m_unique_touches containsObject:touch])
        {
            CGPoint point = [touch locationInView:self];
            glm::ivec2 current_touch_point = glm::ivec2(point.x * self.m_scale_x, point.y * self.m_scale_y);
            ui32 index = static_cast<ui32>([self.m_unique_touches count]);
            self.m_context->gr_pressed(current_touch_point,
                                       self.m_context->get_touch_area_size(),
                                       gb::e_input_source_mouse_left,
                                       index);
            self.m_context->set_previous_touch_point(current_touch_point);
            [self.m_unique_touches addObject:touch];
            [self.m_touches_indexes setObject:touch forKey:@(index)];
        }
    }
}

- (void)touchesMoved:(NSSet*)aTouches withEvent:(UIEvent*)anEvent
{
    assert(self.m_context != nullptr);
    for(UITouch* touch in aTouches)
    {
        NSSet* keys = [self.m_touches_indexes keysOfEntriesPassingTest:^BOOL(NSNumber* key, UITouch *aTouch, BOOL *stop) {
            return [aTouch isEqual:touch];
        }];
        if([keys count] == 1)
        {
            CGPoint point = [touch locationInView:self];
            glm::ivec2 current_touch_point = glm::ivec2(point.x * self.m_scale_x, point.y * self.m_scale_y);
            glm::ivec2 delta = current_touch_point - self.m_context->get_previous_touch_point();
            self.m_context->gr_dragged(current_touch_point,
                                       self.m_context->get_touch_area_size(),
                                       delta,
                                       gb::e_input_source_mouse_left,
                                       static_cast<ui32>([[keys anyObject] unsignedIntegerValue]));
            self.m_context->set_previous_touch_point(current_touch_point);
        }
    }
}

- (void)touchesEnded:(NSSet*)aTouches withEvent:(UIEvent*)anEvent
{
    assert(self.m_context != nullptr);
    for(UITouch* touch in aTouches)
    {
        NSSet* keys = [self.m_touches_indexes keysOfEntriesPassingTest:^BOOL(NSNumber* key, UITouch *aTouch, BOOL *stop) {
            return [aTouch isEqual:touch];
        }];
        if([keys count] == 1)
        {
            CGPoint point = [touch locationInView:self];
            glm::ivec2 current_touch_point = glm::ivec2(point.x * self.m_scale_x, point.y * self.m_scale_y);
            self.m_context->gr_released(current_touch_point,
                                        self.m_context->get_touch_area_size(),
                                        gb::e_input_source_mouse_left,
                                        static_cast<ui32>([[keys anyObject] unsignedIntegerValue]));
            self.m_context->set_previous_touch_point(current_touch_point);
            
            [self.m_unique_touches removeObject:touch];
            [self.m_touches_indexes removeObjectForKey:[keys anyObject]];
        }
        else
        {
            [self.m_unique_touches removeAllObjects];
            [self.m_touches_indexes removeAllObjects];
        }
    }
}

- (void)touchesCancelled:(NSSet*)aTouches withEvent:(UIEvent*)anEvent
{
    assert(self.m_context != nullptr);
    for(UITouch* touch in aTouches)
    {
        NSSet* keys = [self.m_touches_indexes keysOfEntriesPassingTest:^BOOL(NSNumber* key, UITouch *aTouch, BOOL *stop) {
            return [aTouch isEqual:touch];
        }];
        if([keys count] == 1)
        {
            CGPoint point = [touch locationInView:self];
            glm::ivec2 current_touch_point = glm::ivec2(point.x * self.m_scale_x, point.y * self.m_scale_y);
            self.m_context->gr_released(current_touch_point,
                                        self.m_context->get_touch_area_size(),
                                        gb::e_input_source_mouse_left,
                                        static_cast<ui32>([[keys anyObject] unsignedIntegerValue]));
            self.m_context->set_previous_touch_point(current_touch_point);
            
            [self.m_unique_touches removeObject:touch];
            [self.m_touches_indexes removeObjectForKey:[keys anyObject]];
        }
        else
        {
            [self.m_unique_touches removeAllObjects];
            [self.m_touches_indexes removeAllObjects];
        }
    }
}

- (void)showKeyboard
{
    [self becomeFirstResponder];
}

- (void)hideKeyboard
{
    [self resignFirstResponder];
}

@end

namespace gb
{
    class input_context_ios : public input_context
    {
    private:
        
    protected:
        
        void* m_hwnd;
        
    public:
        
        input_context_ios(const std::shared_ptr<window_impl>& window);
        ~input_context_ios();
        
        void show_virtual_keyboard();
        void hide_virtual_keyboard();
    };
    
    std::shared_ptr<input_context> create_input_context_ios(const std::shared_ptr<window_impl>& window)
    {
        return std::make_shared<input_context_ios>(window);
    };
    
    input_context_ios::input_context_ios(const std::shared_ptr<window_impl>& window)
    {
        UIView* view = (__bridge UIView*)window->get_hwnd();
        
        const auto screen_bounds = glm::ivec2([[UIScreen mainScreen] bounds].size.width,
                                                     [[UIScreen mainScreen] bounds].size.height);
        
        input_hwnd* input_view = [[input_hwnd alloc] init];
        input_view.m_context = this;
        input_view.multipleTouchEnabled = YES;
        input_view.frame = CGRectMake(0.f, 0.f,
                                      screen_bounds.x,
                                      screen_bounds.y);
        input_view.m_scale_x = static_cast<f32>(window->get_resolution_size_in_points().x) / static_cast<f32>(screen_bounds.x);
        input_view.m_scale_y = static_cast<f32>(window->get_resolution_size_in_points().y) / static_cast<f32>(screen_bounds.y);
        [view addSubview:input_view];
        m_hwnd = (void *)CFBridgingRetain(input_view);
        
        m_show_virtual_keyboard_callback = std::bind(&input_context_ios::show_virtual_keyboard, this);
        m_hide_virtual_keyboard_callback = std::bind(&input_context_ios::hide_virtual_keyboard, this);
    }
    
    input_context_ios::~input_context_ios()
    {
        
    }
    
    void input_context_ios::show_virtual_keyboard()
    {
        input_hwnd* input_view = (__bridge input_hwnd*)m_hwnd;
        [input_view showKeyboard];
    }
    
    void input_context_ios::hide_virtual_keyboard()
    {
        input_hwnd* input_view = (__bridge input_hwnd*)m_hwnd;
        [input_view hideKeyboard];
    }
}
#endif


