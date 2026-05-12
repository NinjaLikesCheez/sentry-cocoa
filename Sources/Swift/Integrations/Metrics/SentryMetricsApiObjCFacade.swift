import Foundation
import SentryObjCTypes

/// Objective-C-facing facade for the metrics API.
///
/// The canonical Swift entry point — ``SentrySDK/metrics`` — is typed as
/// ``SentryMetricsApiProtocol``, which is not `@objc`-compatible because its
/// `attributes` parameter uses the Swift-only ``SentryAttributeValue`` protocol
/// existential. This class wraps the protocol-typed value with `@objc`-typed
/// overloads that take ``SentryObjCAttributeContent`` instead, so consumers
/// using `-fmodules=NO` (or any pure ObjC client) can call the same selectors
/// as if `metrics` were a regular ObjC class.
///
/// The Swift class is named ``SentryMetricsApiObjCFacade`` to avoid colliding
/// with the existing internal generic struct ``SentryMetricsApi`` (case-
/// insensitive macOS filesystems would otherwise overwrite the source file).
/// The Objective-C runtime name is `SentryMetricsApi` (matching the
/// hand-written wrapper header `Sources/SentryObjC/Public/SentryMetricsApi.h`)
/// so consumers can write
/// `SentryMetricsApi *metrics = [SentrySDK metrics];` and call
/// `[metrics countWithKey:value:attributes:]` exactly as if it were the
/// underlying Swift API.
@objc(SentryMetricsApi)
public final class SentryMetricsApiObjCFacade: NSObject, SentryMetricsApiProtocol {

    /// Shared instance returned by ``SentrySDK/metrics``.
    @objc public static let shared = SentryMetricsApiObjCFacade()

    private let underlying: any SentryMetricsApiProtocol

    override convenience init() {
        self.init(underlying: SentryMetricsApi(dependencies: SentryDependencyContainer.sharedInstance()))
    }

    init(underlying: any SentryMetricsApiProtocol) {
        self.underlying = underlying
    }

    // MARK: - SentryMetricsApiProtocol (Swift-native)

    public func count(
        key: String,
        value: UInt,
        attributes: [String: any SentryAttributeValue]
    ) {
        underlying.count(key: key, value: value, attributes: attributes)
    }

    public func distribution(
        key: String,
        value: Double,
        unit: SentryUnit?,
        attributes: [String: any SentryAttributeValue]
    ) {
        underlying.distribution(key: key, value: value, unit: unit, attributes: attributes)
    }

    public func gauge(
        key: String,
        value: Double,
        unit: SentryUnit?,
        attributes: [String: any SentryAttributeValue]
    ) {
        underlying.gauge(key: key, value: value, unit: unit, attributes: attributes)
    }

    // MARK: - Objective-C entry points
    //
    // Selectors below match the hand-written declarations in
    // `Sources/SentryObjC/Public/SentryMetricsApi.h`. They accept the ObjC
    // value type (`SentryObjCAttributeContent`) and convert to the Swift
    // protocol existential before delegating to the underlying API.

    @objc(countWithKey:value:attributes:)
    public func _objcCount(
        key: String,
        value: UInt,
        attributes: [String: SentryObjCAttributeContent]?
    ) {
        underlying.count(
            key: key,
            value: value,
            attributes: Self.toSwiftAttributes(attributes)
        )
    }

    @objc(distributionWithKey:value:unit:attributes:)
    public func _objcDistribution(
        key: String,
        value: Double,
        unit: String?,
        attributes: [String: SentryObjCAttributeContent]?
    ) {
        underlying.distribution(
            key: key,
            value: value,
            unit: unit.flatMap { SentryUnit(rawValue: $0) },
            attributes: Self.toSwiftAttributes(attributes)
        )
    }

    @objc(gaugeWithKey:value:unit:attributes:)
    public func _objcGauge(
        key: String,
        value: Double,
        unit: String?,
        attributes: [String: SentryObjCAttributeContent]?
    ) {
        underlying.gauge(
            key: key,
            value: value,
            unit: unit.flatMap { SentryUnit(rawValue: $0) },
            attributes: Self.toSwiftAttributes(attributes)
        )
    }

    private static func toSwiftAttributes(
        _ attributes: [String: SentryObjCAttributeContent]?
    ) -> [String: any SentryAttributeValue] {
        guard let attributes else { return [:] }
        return attributes.mapValues { $0.toSwift() }
    }
}
