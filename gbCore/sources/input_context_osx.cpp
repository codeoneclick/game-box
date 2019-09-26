//
//  input_context_osx.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/31/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "input_context.h"

#if defined(__OSX__) && USED_GRAPHICS_API != NO_GRAPHICS_API

#include "window_impl.h"

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>
#import <Carbon/Carbon.h>

@interface input_hwnd : NSView

@property(nonatomic, unsafe_unretained) gb::input_context* m_context;

@end

@implementation input_hwnd

- (instancetype)init
{
    self = [super init];
    if (self)
    {
        
    }
    return self;
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (void)mouseDown:(NSEvent*)event
{
    CGPoint point = [self convertPoint:[event locationInWindow] fromView:nil];
    glm::ivec2 current_touch_point = glm::ivec2(point.x, self.frame.size.height - point.y);
    self.m_context->gr_pressed(current_touch_point, self.m_context->get_touch_area_size(), gb::e_input_source_mouse_left, 0);
    self.m_context->set_previous_touch_point(current_touch_point);
}

- (void)rightMouseDown:(NSEvent *)event;
{
    CGPoint point = [self convertPoint:[event locationInWindow] fromView:nil];
    glm::ivec2 current_touch_point = glm::ivec2(point.x, self.frame.size.height - point.y);
    self.m_context->gr_pressed(current_touch_point, self.m_context->get_touch_area_size(), gb::e_input_source_mouse_right, 0);
    self.m_context->set_previous_touch_point(current_touch_point);
}

- (void)mouseUp:(NSEvent *)event;
{
    CGPoint point = [self convertPoint:[event locationInWindow] fromView:nil];
    glm::ivec2 current_touch_point = glm::ivec2(point.x, self.frame.size.height - point.y);
    self.m_context->gr_released(current_touch_point, self.m_context->get_touch_area_size(), gb::e_input_source_mouse_left, 0);
    self.m_context->set_previous_touch_point(current_touch_point);
}

- (void)rightMouseUp:(NSEvent *)event;
{
    CGPoint point = [self convertPoint:[event locationInWindow] fromView:nil];
    glm::ivec2 current_touch_point = glm::ivec2(point.x, self.frame.size.height - point.y);
    self.m_context->gr_released(current_touch_point, self.m_context->get_touch_area_size(), gb::e_input_source_mouse_right, 0);
    self.m_context->set_previous_touch_point(current_touch_point);
}

- (void)mouseDragged:(NSEvent *)event;
{
    CGPoint point = [self convertPoint:[event locationInWindow] fromView:nil];
    glm::ivec2 current_touch_point = glm::ivec2(point.x, self.frame.size.height - point.y);
    glm::ivec2 delta = current_touch_point - self.m_context->get_previous_touch_point();
    self.m_context->gr_dragged(current_touch_point, self.m_context->get_touch_area_size(), delta, gb::e_input_source_mouse_left, 0);
    self.m_context->set_previous_touch_point(current_touch_point);
}

- (void)rightMouseDragged:(NSEvent *)event;
{
    CGPoint point = [self convertPoint:[event locationInWindow] fromView:nil];
    glm::ivec2 current_touch_point = glm::ivec2(point.x, self.frame.size.height - point.y);
    glm::ivec2 delta = current_touch_point - self.m_context->get_previous_touch_point();
    self.m_context->gr_dragged(current_touch_point, self.m_context->get_touch_area_size(), delta, gb::e_input_source_mouse_right, 0);
    self.m_context->set_previous_touch_point(current_touch_point);
}

- (void)mouseMoved:(NSEvent *)event
{
    CGPoint point = [self convertPoint:[event locationInWindow] fromView:nil];
    glm::ivec2 current_touch_point = glm::ivec2(point.x, self.frame.size.height - point.y);
    glm::ivec2 delta = current_touch_point - self.m_context->get_previous_touch_point();
    self.m_context->gr_moved(current_touch_point, self.m_context->get_touch_area_size(), delta, 0);
    self.m_context->set_previous_touch_point(current_touch_point);
}

- (void)keyDown:(NSEvent *)event
{
    self.m_context->key_down(event.keyCode);
}

- (void)keyUp:(NSEvent *)event
{
    self.m_context->key_up(event.keyCode);
}

@end

namespace gb
{
    class input_context_osx : public input_context
    {
    private:
        
    protected:
        
    public:
        
        input_context_osx(const std::shared_ptr<window_impl>& window);
        ~input_context_osx();
    };
    
    std::shared_ptr<input_context> create_input_context_osx(const std::shared_ptr<window_impl>& window)
    {
        return std::make_shared<input_context_osx>(window);
    };
    
    input_context_osx::input_context_osx(const std::shared_ptr<window_impl>& window)
    {
        NSView* view = (__bridge NSView*)window->get_hwnd();
        
        input_hwnd* input_view = [[input_hwnd alloc] init];
        input_view.m_context = this;
        input_view.frame = CGRectMake(0.f, 0.f,
                                      view.frame.size.width, view.frame.size.height);
        [view addSubview:input_view];
        [input_view becomeFirstResponder];
    }
    
    input_context_osx::~input_context_osx()
    {
        
    }
}

#endif
