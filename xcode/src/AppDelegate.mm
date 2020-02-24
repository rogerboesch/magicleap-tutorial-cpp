
#import "AppDelegate.h"
#import "AppRenderView.h"
#import "RBConsoleView.h"
#import "RBSplitView.h"
#import "RBToolbar.h"

CGFloat gameWidth = 1024;
CGFloat gameHeight = 768;

@interface AppDelegate ()

@property (strong, nonatomic) NSWindow* window;
@property (strong, nonatomic) NSMenu* menu;
@property (strong, nonatomic) AppRenderView* renderView;
@property (strong, nonatomic) RBConsoleView* consoleView;
@property (strong, nonatomic) RBSplitView *topSplitView;

@end

@implementation AppDelegate

// -----------------------------------------------------------------------------
#pragma mark - Toolbar

- (void)itemPressed:(NSString *)name {
	if ([name isEqualToString:@"clearlog"]) {
		[self.consoleView clear];
	}
}

- (void)createToolbar {
    RBToolbar *toolbar = [[RBToolbar alloc] initWithIdentifier:@"AppToolbar"];
    toolbar.callback = self;

    [toolbar addItem:@"clearlog" image:[NSImage imageNamed:NSImageNameFlowViewTemplate] label:@"Clear Log"];

    self.window.toolbar = toolbar;
}


// -----------------------------------------------------------------------------

#pragma mark - Menus

- (void)createMiniMenu {
    self.menu = [[NSMenu alloc] initWithTitle:@"MainMenu"];
    NSApp.mainMenu = self.menu;
    
    NSMenuItem *submenu = [self.menu addItemWithTitle:@"Application" action:nil keyEquivalent:@""];
    NSMenu *menu = [[NSMenu alloc] initWithTitle:@"Application"];
    [self.menu setSubmenu:menu forItem:submenu];

    NSString* title = @"Quit";
    NSMenuItem *item = [menu addItemWithTitle:title action:@selector(terminate:) keyEquivalent:@"q"];
    item.target = NSApp;
}

// -----------------------------------------------------------------------------

#pragma mark - Split view delegate

- (void)splitView:(NSSplitView *)splitView resizeSubviewsWithOldSize:(NSSize)oldSize {
    RBSplitView* rbSplitView = (RBSplitView *)splitView;

    //NSWindow* window = rbSplitView.window;
    //CGRect windowRect = [NSWindow contentRectForFrameRect:[window frame] styleMask:[window styleMask]];
    //CGRect contentRect = window.contentView.frame;
	int toolbarHeight = 0; //NSHeight(windowRect) - NSHeight(contentRect);

	if (rbSplitView.userTag == 1) {
        NSView* leftView = [splitView.subviews objectAtIndex:0];
        NSView* rightView = [splitView.subviews objectAtIndex:1];
        
        CGFloat leftWidth = (splitView.bounds.size.width-splitView.dividerThickness) / 2;
        
        NSRect leftFrame = NSMakeRect(0, toolbarHeight, leftWidth, splitView.frame.size.height-toolbarHeight);
        NSRect rightFrame = NSMakeRect(leftWidth+splitView.dividerThickness, toolbarHeight, splitView.frame.size.width-leftWidth-splitView.dividerThickness, splitView.frame.size.height-toolbarHeight);
        		
        leftView.frame = leftFrame;
        rightView.frame = rightFrame;

		[self.renderView resize:leftFrame.size];
	}
}

- (NSRect)splitView:(NSSplitView *)splitView effectiveRect:(NSRect)proposedEffectiveRect forDrawnRect:(NSRect)drawnRect ofDividerAtIndex:(NSInteger)dividerIndex {
    return NSMakeRect(0, 0, 0, 0);
}

- (void)windowDidResize:(NSNotification *)notification {
	self.topSplitView.frame = self.window.contentView.bounds;
}

// -----------------------------------------------------------------------------

#pragma mark - Content View

- (void)createContentView {
	// Create at first
	self.consoleView = [[RBConsoleView alloc] initWithFrame:self.window.contentView.bounds];

	self.topSplitView = [[RBSplitView alloc] initWithFrame:self.window.contentView.bounds];
    [self.topSplitView setVertical:true];
    self.topSplitView.dividerStyle = NSSplitViewDividerStyleThin;
    self.topSplitView.delegate = self;
    self.topSplitView.userTag = 1;
    [self.window.contentView addSubview:self.topSplitView];
		
	self.renderView = [[AppRenderView alloc] initWithFrame:self.window.contentView.bounds];
    self.renderView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    [self.topSplitView addSubview:self.renderView];

    self.consoleView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    [self.topSplitView addSubview:self.consoleView];
}

// -----------------------------------------------------------------------------

- (void)createWindow:(NSString *)title {
    NSRect contentRect = NSMakeRect(0, 0, 1024, 768);
    NSWindowStyleMask styleMask = NSWindowStyleMaskTitled|NSWindowStyleMaskClosable|NSWindowStyleMaskMiniaturizable|NSWindowStyleMaskResizable;

    self.window = [[NSWindow alloc] initWithContentRect:contentRect styleMask:styleMask backing:NSBackingStoreBuffered defer:true];
    self.window.title = title;
    self.window.delegate = self;

    [self createContentView];
    [self createToolbar];
    
    [self.window makeKeyAndOrderFront:nil];
}

// -----------------------------------------------------------------------------

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    [self createWindow:@"oglIDE | Magic Leap 1 Edition"];
    [self createMiniMenu];
        
    [self.renderView start];
}

// -----------------------------------------------------------------------------

- (void)applicationWillTerminate:(NSNotification *)aNotification {
}

@end
