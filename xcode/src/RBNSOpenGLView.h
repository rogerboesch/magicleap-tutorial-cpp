
#import <Cocoa/Cocoa.h>
#import <CoreVideo/CoreVideo.h>

@interface RBNSOpenGLView : NSOpenGLView {
    CVDisplayLinkRef _displayLink;
}

- (void)render;
- (void)renderLoop;
- (void)initialize:(CGSize)size;
- (void)resize:(CGSize)size;

- (void)start;
- (void)stop;

@end
