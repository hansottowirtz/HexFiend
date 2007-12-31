//
//  HFController.h
//  HexFiend_2
//
//  Created by Peter Ammon on 11/3/07.
//  Copyright 2007 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import <HexFiend/HFTypes.h>

@class HFRepresenter, HFByteArray, HFControllerCoalescedUndo;

enum
{
    HFControllerContentValue = 1 << 0,
    HFControllerContentLength = 1 << 1,
    HFControllerDisplayedRange = 1 << 2,
    HFControllerSelectedRanges = 1 << 3,
    HFControllerBytesPerLine = 1 << 4,
    HFControllerEditable = 1 << 5,
    HFControllerFont = 1 << 6,
    HFControllerLineHeight = 1 << 7 
};
typedef NSUInteger HFControllerPropertyBits;

enum
{
    HFControllerDirectionLeft,
    HFControllerDirectionRight,
    HFControllerDirectionUp,
    HFControllerDirectionDown
};
typedef NSUInteger HFControllerMovementDirection;

enum
{
    HFControllerMovementByte,
    HFControllerMovementLine,
    HFControllerMovementPage,
    HFControllerMovementDocument
};
typedef NSUInteger HFControllerMovementQuantity;

@interface HFController : NSObject {
    @private
    NSMutableArray *representers;
    HFByteArray *byteArray;
    NSMutableArray *selectedContentsRanges;
    HFRange displayedContentsRange;
    HFFPRange displayedLineRange;
    NSUInteger bytesPerLine;
    NSFont *font;
    CGFloat lineHeight;
    
    NSUInteger currentPropertyChangeToken;
    HFControllerPropertyBits propertiesToUpdateInCurrentTransaction;
    
    NSUndoManager *undoManager;
    
    unsigned long long selectionAnchor;
    HFRange selectionAnchorRange;
    
    unsigned long long typingUndoOptimizationInsertionLocation;
    HFControllerCoalescedUndo *coalescedUndo;
    
    struct  {
        unsigned editable:1;
        unsigned selectable:1;
        unsigned selectionInProgress:1;
        unsigned shiftExtendSelection:1;
        unsigned commandExtendSelection:1;
        unsigned reserved1:27;
        unsigned reserved2:32;
    } _hfflags;
}

/* Methods for dealing with representers */
- (NSArray *)representers;
- (void)addRepresenter:(HFRepresenter *)representer;
- (void)removeRepresenter:(HFRepresenter *)representer;

/* Property transaction methods.  There is a property transaction stack, and all property changes are collected until the last token is popped off the stack, at which point all representers are notified of all collected changes via viewChangedProperties:.  Tokens cannot be popped out of order - they are used as a correctness check. */
- (NSUInteger)beginPropertyChangeTransaction;
- (void)endPropertyChangeTransaction:(NSUInteger)token;

- (HFFPRange)displayedLineRange;
- (void)setDisplayedLineRange:(HFFPRange)range;

/* Returns all lines on which the cursor may be placed.  This is equivalent to (unsigned long long)(HFRoundUpToNextMultiple(contentsLength, bytesPerLine) / bytesPerLine) */
- (unsigned long long)totalLineCount;

/* Methods for obtaining information about the current contents state */
- (HFRange)displayedContentsRange;
- (void)setDisplayedContentsRange:(HFRange)range;

- (NSFont *)font;
- (void)setFont:(NSFont *)font;

- (CGFloat)lineHeight;

- (NSArray *)selectedContentsRanges; //returns an array of HFRangeWrappers
- (unsigned long long)contentsLength; //returns total length of contents

/* Methods for getting at data */
- (NSData *)dataForRange:(HFRange)range;
- (void)copyBytes:(unsigned char *)bytes range:(HFRange)range;

/* Methods for setting a byte array */
- (void)setByteArray:(HFByteArray *)val;
- (HFByteArray *)byteArray;

/* Methods for setting an undo manager.  If one is not set, undo does not occur. */
- (void)setUndoManager:(NSUndoManager *)manager;
- (NSUndoManager *)undoManager;

/* Set/get editable property */
- (BOOL)isEditable;
- (void)setEditable:(BOOL)flag;

/* Line oriented representers can use this */
- (NSUInteger)bytesPerLine;

/* Callback for a representer-initiated change to some property */
- (void)representer:(HFRepresenter *)rep changedProperties:(HFControllerPropertyBits)properties;

/* Selection methods */
- (void)beginSelectionWithEvent:(NSEvent *)event forByteIndex:(unsigned long long)byteIndex;
- (void)continueSelectionWithEvent:(NSEvent *)event forByteIndex:(unsigned long long)byteIndex;
- (void)endSelectionWithEvent:(NSEvent *)event forByteIndex:(unsigned long long)byteIndex;

/* Scroll wheel support */
- (void)scrollWithScrollEvent:(NSEvent *)scrollEvent;

/* Action methods */
- (IBAction)selectAll:sender;

/* Keyboard navigation */
- (void)moveDirection:(HFControllerMovementDirection)direction andModifySelection:(BOOL)extendSelection;

/* Text editing.  All of the following methods are undoable. */

/* Replaces the selection with the given data. */
- (void)insertData:(NSData *)data;

/* Deletes the selection */
- (void)deleteSelection;

/* Deletes one byte in a given direction, which must be HFControllerDirectionLeft or HFControllerDirectionRight */
- (void)deleteDirection:(HFControllerMovementDirection)direction;

@end
