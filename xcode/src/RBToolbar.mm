
#import "RBToolbar.h"

@interface RBToolbar ()

@property (strong, nonatomic) NSMutableDictionary* entries; // String, Toolbar item
@property (strong, nonatomic) NSMutableArray* identifiers;  // Tooldbar ident

@end

@implementation RBToolbar


// -----------------------------------------------------------------------------

#pragma mark - Actions

- (void)buttonPressed:(NSButton *)sender {
    NSString *name = self.identifiers[sender.tag-1];
    
    if (self.callback != nil) {
        [self.callback itemPressed:name];
    }
}

// -----------------------------------------------------------------------------

- (void)segmentPressed:(NSSegmentedControl *)sender {
    NSString *name = self.identifiers[sender.tag-1];
    
    if (self.callback != nil) {
        [self.callback itemPressed:name];
    }
}

// -----------------------------------------------------------------------------

#pragma mark - Change items

- (void)setLabel:(NSString *)label forItem:(NSString *)name {
    NSToolbarItem *item = [self.entries objectForKey:name];
    if (item == nil) {
        return;
    }

    item.label = label;
}

// -----------------------------------------------------------------------------

- (void)setImage:(NSImage *)image forItem:(NSString *)name {
    NSToolbarItem *item = [self.entries objectForKey:name];
    if (item == nil) {
        return;
    }
    
    if ([item.view isKindOfClass:[NSButton class]]) {
        NSButton *button = (NSButton *)item.view;
        button.image = image;
    }
}

// -----------------------------------------------------------------------------

- (NSSegmentedControl *)segmentForItem:(NSString *)name {
    NSToolbarItem* item = [self.entries objectForKey:name];
    if (item == nil) {
        return nil;
    }
    
    if ([item.view isKindOfClass:[NSSegmentedControl class]]) {
        return (NSSegmentedControl *)item.view;
    }
    
    return nil;
}

// -----------------------------------------------------------------------------

- (void)enableSegment:(NSString *)name enable:(BOOL)enable {
    NSSegmentedControl *segment = [self segmentForItem:name];
    if (segment != nil) {
        [segment setEnabled:enable];
    }
}

// -----------------------------------------------------------------------------

- (int)selectionForSegment:(NSString *)name {
    NSSegmentedControl *segment = [self segmentForItem:name];
    if (segment == nil) {
        return -1;
    }
    
    return (int)segment.selectedSegment;
}

- (void)selectSegment:(NSString *)name index:(int)index {
    NSSegmentedControl *segment = [self segmentForItem:name];
    if (segment == nil) {
        return;
    }
	
	[segment setSelectedSegment:index];
}

// -----------------------------------------------------------------------------

#pragma mark - Add items

- (void)addItem:(NSString *)name image:(NSImage *)image label:(NSString *)label {
    NSToolbarItem* item = [self.entries objectForKey:name];
    if (item != nil) {
        return;
    }
    
    // Add itentifier
    [self.identifiers addObject:name];
    
    NSButton *button = [[NSButton alloc] initWithFrame:NSMakeRect(0, 0, 50, 32)];
    button.image = image;
    button.target = self;
    button.action = @selector(buttonPressed:);
    button.bezelStyle = NSBezelStyleTexturedSquare;
    button.tag = self.identifiers.count;
    
    // Add Item
    item = [[NSToolbarItem alloc] initWithItemIdentifier:name];
    item.label = label;
    item.view = button;
    [self.entries setObject:item forKey:name];
}

// -----------------------------------------------------------------------------

- (void)addSegment:(NSString *)name labels:(NSArray *)labels width:(CGFloat)width title:(NSString *)title {
    NSToolbarItem* item = [self.entries objectForKey:name];
    if (item != nil) {
        return;
    }
    
    // Add itentifier
    [self.identifiers addObject:name];
    
    // Create segment
    NSSegmentedControl *segment = [NSSegmentedControl segmentedControlWithLabels:labels trackingMode:NSSegmentSwitchTrackingSelectOne target:self action:@selector(segmentPressed:)];
    [segment setSelected:true forSegment:0];
    segment.frame = NSMakeRect(0, 0, CGFloat(labels.count)*width+10, 32);
    segment.segmentStyle = NSSegmentStyleTexturedRounded;
    segment.tag = self.identifiers.count;

    // Add Item
    item = [[NSToolbarItem alloc] initWithItemIdentifier:name];
    item.label = title;
    item.view = segment;
    [self.entries setObject:item forKey:name];
}

- (void)addSpace:(int)width {
	NSString *name = [NSString stringWithFormat:@"space%d", width];
    NSToolbarItem* item = [self.entries objectForKey:name];
    
	if (item != nil) {
		name = [NSString stringWithFormat:@"space%d%d", width, (int)self.entries.count];
    }
    
    // Add itentifier
    [self.identifiers addObject:name];
    
    NSView *view = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, width, 32)];
    
    // Add Item
    item = [[NSToolbarItem alloc] initWithItemIdentifier:name];
    item.view = view;
    [self.entries setObject:item forKey:name];
}

// -----------------------------------------------------------------------------

#pragma mark - Delegate

- (NSToolbarItem *)toolbar:(NSToolbar *)toolbar itemForItemIdentifier:(NSToolbarItemIdentifier)itemIdentifier willBeInsertedIntoToolbar:(BOOL)flag {
    return [self.entries objectForKey:itemIdentifier];
}

// -------------------------------------------------------------------------

- (NSArray<NSToolbarItemIdentifier> *)toolbarAllowedItemIdentifiers:(NSToolbar *)toolbar {
    return self.identifiers;
}

// -----------------------------------------------------------------------------

- (NSArray<NSToolbarItemIdentifier> *)toolbarDefaultItemIdentifiers:(NSToolbar *)toolbar {
    return self.identifiers;
}

// -----------------------------------------------------------------------------

- (NSArray<NSToolbarItemIdentifier> *)toolbarSelectableItemIdentifiers:(NSToolbar *)toolbar {
    return self.identifiers;
}

// -----------------------------------------------------------------------------

#pragma mark - Initialisation

- (id)initWithIdentifier:(NSString *)name {
    self = [super initWithIdentifier:name];

    self.allowsUserCustomization = false;
    self.delegate = self;
    
    self.entries = [NSMutableDictionary new];
    self.identifiers = [NSMutableArray new];

    return self;
}

@end
