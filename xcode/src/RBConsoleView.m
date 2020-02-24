
#import "RBConsoleView.h"

#define MAX_BUFFER_SIZE 100

@interface RBConsoleView ()

@property (nonatomic) int lineNumber;


@property (strong, nonatomic) NSScrollView* scrollView;
@property (strong, nonatomic) NSTextView* textView;
@property (strong, nonatomic) NSMutableArray* buffer;

+ (RBConsoleView *)shared;

@end

void console_print(char* message) {
	NSString* str = [NSString stringWithUTF8String:message];
	[[RBConsoleView shared] println:str];
}

@implementation RBConsoleView

// -----------------------------------------------------------------------------
#pragma mark - Print

- (void)clear {
	[self.buffer removeAllObjects];
	self.lineNumber = 0;
	
	dispatch_async(dispatch_get_main_queue(), ^{
		[self.textView setString:@""];
	});
}

- (void)print:(NSString *)msg {
	[self.buffer addObject:msg];

	if (self.buffer.count > MAX_BUFFER_SIZE) {
		[self.buffer removeObjectAtIndex:0];
	}

	NSMutableString *output = [NSMutableString string];
	for (NSString* str in self.buffer) {
		[output appendString:str];
	}
	
	dispatch_async(dispatch_get_main_queue(), ^{
		[self.textView setString:output];
	});
}

- (void)println:(NSString *)msg {
	NSString *str = [NSString stringWithFormat:@"%05d> %@\n", ++self.lineNumber, msg];
	[self print:str];
}

- (void)resizeSubviewsWithOldSize:(NSSize)oldSize {
	NSRect rect = self.bounds;
	self.scrollView.frame = rect;
	self.textView.frame = rect;
}

// -----------------------------------------------------------------------------
#pragma mark - Initialisation

RBConsoleView* shareInstance = NULL;

+ (RBConsoleView *)shared {
	return shareInstance;
}

- (id)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
	
	shareInstance = self;

	self.wantsLayer = true;
    self.layer.backgroundColor = [NSColor blackColor].CGColor;

	self.scrollView = [[NSScrollView alloc] initWithFrame:frame];
	self.scrollView.wantsLayer = true;
	[self addSubview:self.scrollView];

	self.textView = [[NSTextView alloc] initWithFrame:frame];
	self.textView.wantsLayer = true;
	self.textView.drawsBackground = false;
	self.textView.font = [NSFont  fontWithName:@"Andale Mono" size: 12];
	self.textView.textColor = [NSColor greenColor];
	[self.textView setEditable:NO];
	self.textView.alignment = NSTextAlignmentLeft;
	
	[self.scrollView addSubview: self.textView];
	self.scrollView.documentView = self.textView;

	self.buffer = [NSMutableArray new];

	return self;
}

@end
