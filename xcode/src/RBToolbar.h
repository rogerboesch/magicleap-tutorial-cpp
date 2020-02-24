
#import <Cocoa/Cocoa.h>

@protocol RBToolbarDelegate

- (void)itemPressed:(NSString *)name;

@end

@interface RBToolbar : NSToolbar <NSToolbarDelegate> {
    
}

@property (nonatomic, assign) id<RBToolbarDelegate> callback;

- (void)setLabel:(NSString *)label forItem:(NSString *)name;
- (void)setImage:(NSImage *)image forItem:(NSString *)name;

- (void)addSpace:(int)width;

- (void)addItem:(NSString *)name image:(NSImage *)image label:(NSString *)label;

- (void)addSegment:(NSString *)name labels:(NSArray *)labels width:(CGFloat)width title:(NSString *)title;
- (void)enableSegment:(NSString *)name enable:(BOOL)enable;
- (int)selectionForSegment:(NSString *)name;
- (void)selectSegment:(NSString *)name index:(int)index;

@end
