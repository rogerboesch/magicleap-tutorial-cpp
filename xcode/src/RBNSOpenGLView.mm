
#import "RBNSOpenGLView.h"
#import <OpenGL/gl.h>
#import <OpenGL/OpenGL.h>

@implementation RBNSOpenGLView

// -----------------------------------------------------------------------------
// #pragma mark - User input

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (BOOL)becomeFirstResponder {
    return YES;
}

- (BOOL)resignFirstResponder {
    return YES;
}

- (void)processKey:(int)keyCode pressDown:(bool)down shiftPressed:(bool)shiftPressed {}

- (void)keyDown:(NSEvent *)event {
    NSLog(@"Key down: %d", event.keyCode);

	bool shiftPressed = NO;
	if (event.modifierFlags & NSEventModifierFlagShift) {
		shiftPressed = YES;
	}
	
    [self processKey:event.keyCode pressDown:true shiftPressed:shiftPressed];
}

- (void)keyUp:(NSEvent *)event {
    NSLog(@"Key up: %d", event.keyCode);
    
    [self processKey:event.keyCode pressDown:false shiftPressed:false];
}

// -----------------------------------------------------------------------------
// #pragma mark - Render

- (void)renderLoop {
	[self render];

	CGLContextObj ctx = 0;
	CGLFlushDrawable(ctx);
	glFlush();

    float time = 1.0/60.0;
    [self performSelector:@selector(renderLoop) withObject:nil afterDelay:time];
}

- (void)render {}
- (void)initialize:(CGSize)size {}
- (void)resize:(CGSize)size {}

// -----------------------------------------------------------------------------

- (id)initWithFrame:(NSRect)frame {
    NSOpenGLPixelFormatAttribute pixelFormatAttributes[] = {
        NSOpenGLProfileVersionLegacy,   // Important to work with current shader implementation
        0
    };
    
    NSOpenGLPixelFormat *pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAttributes];
    self = [super initWithFrame:frame pixelFormat:pixelFormat];
    
    [[self openGLContext] makeCurrentContext];

    [self initialize:frame.size];
    
    float time = 1.0/60.0;
    [self performSelector:@selector(renderLoop) withObject:nil afterDelay:time];
    
    return self;
}

- (void)start {
}

- (void)stop {
}

@end
