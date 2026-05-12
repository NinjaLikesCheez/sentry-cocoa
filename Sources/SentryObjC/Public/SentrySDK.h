#import <Foundation/Foundation.h>

#import "SentryLastRunStatus.h"
#import "SentryObjCDefines.h"

@class SentryBreadcrumb;
@class SentryEvent;
@class SentryFeedback;
@class SentryId;
@class SentryLogger;
@class SentryMetricsApi;
@class SentryOptions;
@class SentryScope;
@class SentryTransactionContext;
@class SentryUser;
@protocol SentrySpan;

#if SENTRY_OBJC_REPLAY_SUPPORTED
@class SentryReplayApi;
#endif

#if TARGET_OS_IOS && SENTRY_HAS_UIKIT
@class SentryFeedbackAPI;
#endif

NS_ASSUME_NONNULL_BEGIN

@interface SentrySDK : NSObject

SENTRY_NO_INIT

#pragma mark - Lifecycle

/**
 * Initializes and configures the Sentry SDK with the supplied options.
 *
 * @param options A fully configured @c SentryOptions instance (must include a valid DSN).
 *
 * @note Call on the main thread; if invoked from a background thread, the SDK starts
 *       on the main thread asynchronously.
 */
+ (void)startWithOptions:(SentryOptions *)options NS_SWIFT_NAME(start(options:));

/**
 * Initializes and configures the Sentry SDK using a configuration block.
 *
 * @param configureOptions A block that receives a fresh @c SentryOptions instance to mutate.
 *
 * @note Call on the main thread; if invoked from a background thread, the SDK starts
 *       on the main thread asynchronously.
 */
+ (void)startWithConfigureOptions:(void (^)(SentryOptions *options))configureOptions
    NS_SWIFT_NAME(start(configureOptions:));

#pragma mark - Metrics

/**
 * Returns the metrics API for recording counters, gauges, and distributions.
 *
 * @discussion Backed by the @c \@objc(SentryMetricsApi) Swift class — the
 * selectors @c countWithKey:value:attributes:, @c distributionWithKey:value:unit:attributes:,
 * and @c gaugeWithKey:value:unit:attributes: are exported by Swift and resolved
 * directly by ObjC consumers.
 */
+ (SentryMetricsApi *)metrics;

#pragma mark - Read-only state

/**
 * Returns the currently active span or transaction.
 *
 * @return The active span, or @c nil if no span is active.
 */
+ (nullable id<SentrySpan>)span;

/**
 * Indicates whether the SDK has been started and is enabled.
 */
+ (BOOL)isEnabled;

/**
 * Returns the SDK logger instance.
 */
+ (SentryLogger *)logger;

#if SENTRY_OBJC_REPLAY_SUPPORTED
/**
 * Returns the Session Replay API for controlling replay recording.
 */
+ (SentryReplayApi *)replay;
#endif

#if TARGET_OS_IOS && SENTRY_HAS_UIKIT
/**
 * Returns the API for capturing user feedback.
 */
+ (SentryFeedbackAPI *)feedback;
#endif

#pragma mark - Breadcrumbs and scope

/**
 * Adds a breadcrumb to the current scope.
 */
+ (void)addBreadcrumb:(SentryBreadcrumb *)crumb;

/**
 * Modifies the current scope using a callback block.
 */
+ (void)configureScope:(void (^)(SentryScope *scope))callback;

#pragma mark - Event capture

/**
 * Captures an event and sends it to Sentry.
 */
+ (SentryId *)captureEvent:(SentryEvent *)event;

/**
 * Captures an event with a specific scope.
 */
+ (SentryId *)captureEvent:(SentryEvent *)event withScope:(SentryScope *)scope;

/**
 * Captures an event with a scope modification block.
 */
+ (SentryId *)captureEvent:(SentryEvent *)event withScopeBlock:(void (^)(SentryScope *scope))block;

/**
 * Captures an event with a per-call override for attaching all threads.
 */
+ (SentryId *)captureEvent:(SentryEvent *)event attachAllThreads:(BOOL)attachAllThreads;

/**
 * Captures an @c NSError and sends it to Sentry.
 */
+ (SentryId *)captureError:(NSError *)error;

/**
 * Captures an @c NSError with a specific scope.
 */
+ (SentryId *)captureError:(NSError *)error withScope:(SentryScope *)scope;

/**
 * Captures an @c NSError with a scope modification block.
 */
+ (SentryId *)captureError:(NSError *)error withScopeBlock:(void (^)(SentryScope *scope))block;

/**
 * Captures an @c NSError with a per-call override for attaching all threads.
 */
+ (SentryId *)captureError:(NSError *)error attachAllThreads:(BOOL)attachAllThreads;

/**
 * Captures an @c NSException and sends it to Sentry.
 */
+ (SentryId *)captureException:(NSException *)exception;

/**
 * Captures an @c NSException with a specific scope.
 */
+ (SentryId *)captureException:(NSException *)exception withScope:(SentryScope *)scope;

/**
 * Captures an @c NSException with a scope modification block.
 */
+ (SentryId *)captureException:(NSException *)exception
                withScopeBlock:(void (^)(SentryScope *scope))block;

/**
 * Captures an @c NSException with a per-call override for attaching all threads.
 */
+ (SentryId *)captureException:(NSException *)exception attachAllThreads:(BOOL)attachAllThreads;

/**
 * Captures a message string and sends it to Sentry.
 */
+ (SentryId *)captureMessage:(NSString *)message;

/**
 * Captures a message with a specific scope.
 */
+ (SentryId *)captureMessage:(NSString *)message withScope:(SentryScope *)scope;

/**
 * Captures a message with a scope modification block.
 */
+ (SentryId *)captureMessage:(NSString *)message withScopeBlock:(void (^)(SentryScope *scope))block;

/**
 * Captures a message with a per-call override for attaching all threads.
 */
+ (SentryId *)captureMessage:(NSString *)message attachAllThreads:(BOOL)attachAllThreads;

/**
 * Captures user feedback and sends it to Sentry.
 */
+ (void)captureFeedback:(SentryFeedback *)feedback;

#pragma mark - Performance

/**
 * Starts a new transaction for performance monitoring.
 */
+ (id<SentrySpan>)startTransactionWithName:(NSString *)name operation:(NSString *)operation;

/**
 * Starts a new transaction with scope binding control.
 */
+ (id<SentrySpan>)startTransactionWithName:(NSString *)name
                                 operation:(NSString *)operation
                               bindToScope:(BOOL)bindToScope;

/**
 * Starts a new transaction from a transaction context.
 */
+ (id<SentrySpan>)startTransactionWithContext:(SentryTransactionContext *)transactionContext;

/**
 * Starts a new transaction from a transaction context with scope binding control.
 */
+ (id<SentrySpan>)startTransactionWithContext:(SentryTransactionContext *)transactionContext
                                  bindToScope:(BOOL)bindToScope;

/**
 * Starts a new transaction with custom sampling context.
 */
+ (id<SentrySpan>)startTransactionWithContext:(SentryTransactionContext *)transactionContext
                        customSamplingContext:(NSDictionary<NSString *, id> *)customSamplingContext;

/**
 * Starts a new transaction with full control over binding and sampling.
 */
+ (id<SentrySpan>)startTransactionWithContext:(SentryTransactionContext *)transactionContext
                                  bindToScope:(BOOL)bindToScope
                        customSamplingContext:(NSDictionary<NSString *, id> *)customSamplingContext;

/**
 * Reports that the application is now fully displayed to the user.
 */
+ (void)reportFullyDisplayed;

#pragma mark - User and session

/**
 * Sets the user information for the current scope.
 */
+ (void)setUser:(nullable SentryUser *)user;

/**
 * Manually starts a new session.
 */
+ (void)startSession;

/**
 * Manually ends the current session.
 */
+ (void)endSession;

#pragma mark - Crash detection

/**
 * Indicates whether the application crashed during the last run.
 */
+ (BOOL)crashedLastRun __attribute__((deprecated("Use lastRunStatus instead")));

/**
 * Returns the crash status of the last program execution.
 */
+ (SentryLastRunStatus)lastRunStatus;

/**
 * Indicates whether a startup crash was detected.
 */
+ (BOOL)detectedStartUpCrash;

/**
 * Triggers a crash for testing purposes.
 *
 * @warning Terminates the application.
 */
+ (void)crash;

#pragma mark - App hang tracking

/**
 * Pauses sending detected app hangs to Sentry.
 */
+ (void)pauseAppHangTracking;

/**
 * Resumes sending detected app hangs to Sentry after they were paused.
 */
+ (void)resumeAppHangTracking;

#pragma mark - Lifecycle

/**
 * Waits for all pending events to be sent to Sentry.
 *
 * @param timeout Maximum time to wait in seconds.
 */
+ (void)flush:(NSTimeInterval)timeout;

/**
 * Closes the SDK and uninstalls integrations.
 */
+ (void)close;

#if !(TARGET_OS_WATCH || TARGET_OS_TV || TARGET_OS_VISION)
#    pragma mark - Profiling

/**
 * Manually starts the profiler.
 */
+ (void)startProfiler;

/**
 * Manually stops the profiler.
 */
+ (void)stopProfiler;
#endif

@end

NS_ASSUME_NONNULL_END
