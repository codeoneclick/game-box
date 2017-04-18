//
//  input_context_ios.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/31/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#if defined(__IOS__) && !defined(__NO_RENDER__)

#include "input_context.h"
#include "ogl_window.h"

#include <Foundation/Foundation.h>
#include <UIKit/UIKit.h>

@interface input_hwnd : UIView<UIKeyInput>

@property(nonatomic, unsafe_unretained) gb::input_context* m_context;
@property(nonatomic, strong) NSMutableSet* m_unique_touches;
@property(nonatomic, strong) NSMutableDictionary<NSNumber*, UITouch*>* m_touches_indexes;

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
            self.m_context->gr_pressed(current_touch_point, gb::e_input_source_mouse_left, index);
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
        self.m_context->gr_dragged(current_touch_point, delta, gb::e_input_source_mouse_left,
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
        self.m_context->gr_released(current_touch_point, gb::e_input_source_mouse_left,
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
        self.m_context->gr_released(current_touch_point, gb::e_input_source_mouse_left,
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
        
        input_context_ios(const std::shared_ptr<ogl_window>& window);
        ~input_context_ios();
        
        void show_virtual_keyboard();
        void hide_virtual_keyboard();
    };
    
    std::shared_ptr<input_context> create_input_context_ios(const std::shared_ptr<ogl_window>& window)
    {
        return std::make_shared<input_context_ios>(window);
    };
    
    input_context_ios::input_context_ios(const std::shared_ptr<ogl_window>& window)
    {
        UIView* view = (__bridge UIView*)window->get_hwnd();
        
        input_hwnd* input_view = [[input_hwnd alloc] init];
        input_view.m_context = this;
        input_view.multipleTouchEnabled = YES;
        input_view.frame = CGRectMake(0.f, 0.f,
                                     view.frame.size.width, view.frame.size.height);
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


