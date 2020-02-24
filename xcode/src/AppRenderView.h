
#import <Cocoa/Cocoa.h>
#import "RBNSOpenGLView.h"

@interface AppRenderView : RBNSOpenGLView {
    
}

- (void)render;
- (void)initialize:(CGSize)size;
- (void)resize:(CGSize)size;

@end
