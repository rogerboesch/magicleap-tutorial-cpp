
#import <Cocoa/Cocoa.h>

@interface RBConsoleView : NSView {
	
}

- (void)clear;
- (void)print:(NSString *)msg;
- (void)println:(NSString *)msg;

@end
