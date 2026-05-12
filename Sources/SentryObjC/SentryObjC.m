// SentryObjC is a header-only wrapper. Every `@interface` in `Public/`
// resolves at link time to a Swift `@objc(SentryX)` class shipped by the
// embedded `Sentry` framework (or `SentrySwift` / `SentryObjCInternal` on the
// SPM compile-from-source path). There are no `@implementation` blocks here:
// adding one would register a duplicate ObjC runtime class.
//
// SPM and `libtool` both require at least one source file per target, so this
// translation unit exists purely to satisfy the build system.
#import <Foundation/Foundation.h>
