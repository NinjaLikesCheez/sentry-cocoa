#import <Foundation/Foundation.h>

#import "SentryObjCDefines.h"

@class SentryObjCAttributeContent;

NS_ASSUME_NONNULL_BEGIN

/**
 * Pure-Objective-C facade for the Swift @c SentryMetricsApi class.
 *
 * @discussion Same "same-name" pattern as @c SentrySDK / @c SentryOptions.
 * The Swift class is declared as
 * @code @objc(SentryMetricsApi) public final class SentryMetricsApiObjCFacade: NSObject @endcode
 * (Swift name @c SentryMetricsApiObjCFacade to disambiguate from the internal generic
 * struct @c SentryMetricsApi). The ObjC runtime name is pinned to
 * @c "SentryMetricsApi" so this hand-written @c \@interface declaration
 * resolves at link time to the Swift implementation in @c SentrySwift /
 * @c SentryObjCInternal. There is @b no @c \@implementation in the wrapper.
 *
 * Access via @c [SentrySDK metrics].
 *
 * @see SentryObjCAttributeContent
 * @see SentryObjCUnit.h for predefined unit constants
 */
@interface SentryMetricsApi : NSObject

/**
 * Records a count metric for the specified key.
 *
 * @param key A namespaced identifier for the metric (e.g., @c "network.request.count").
 *            Prefer stable, lowercase, dot-delimited names to aid aggregation and filtering.
 * @param value The count value to recodcrd.
 * @param attributes Optional dictionary of attributes to attach to the metric.
 *                   Keys are strings, values are @c SentryObjCAttributeContent instances.
 */
- (void)countWithKey:(NSString *)key
               value:(NSUInteger)value
          attributes:(nullable NSDictionary<NSString *, SentryObjCAttributeContent *> *)attributes;

/**
 * Records a distribution metric for the specified key.
 *
 * @param key A namespaced identifier for the metric.
 * @param value The value to record in the distribution.
 * @param unit Optional unit of measurement (e.g., @c SentryObjCUnitNameMillisecond).
 * @param attributes Optional dictionary of attributes to attach to the metric.
 */
- (void)distributionWithKey:(NSString *)key
                      value:(double)value
                       unit:(nullable NSString *)unit
                 attributes:
                     (nullable NSDictionary<NSString *, SentryObjCAttributeContent *> *)attributes;

/**
 * Records a gauge metric for the specified key.
 *
 * @param key A namespaced identifier for the metric.
 * @param value The current gauge value to record.
 * @param unit Optional unit of measurement (e.g., @c SentryObjCUnitNameByte).
 * @param attributes Optional dictionary of attributes to attach to the metric.
 */
- (void)gaugeWithKey:(NSString *)key
               value:(double)value
                unit:(nullable NSString *)unit
          attributes:(nullable NSDictionary<NSString *, SentryObjCAttributeContent *> *)attributes;

@end

NS_ASSUME_NONNULL_END
