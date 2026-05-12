import Foundation
@_spi(Private) @testable import Sentry
import SentryObjCTypes
import XCTest

/// Tests for the inline Objective-C bridging that moved from the deleted
/// `SentryObjCBridge` target into the Swift SDK itself:
///
/// - `SentryObjCMetric` ⇄ `SentryMetric` conversion (declared in
///   `Sources/Swift/Integrations/Metrics/SentryObjCTypeConversions.swift`).
/// - The `@objc(setBeforeSendMetric:)` setter on `Options` (declared in
///   `Sources/Swift/Options.swift`), which wraps an ObjC-typed callback in
///   a Swift closure that the SDK can invoke with native `SentryMetric`
///   values.
final class SentryMetricObjCBridgeTests: XCTestCase {

    func testMetricToObjC_shouldIncludeAllProperties() throws {
        let metric = originalMetric()

        let result = metric.toObjC()

        XCTAssertEqual(result.timestamp, metric.timestamp)
        XCTAssertEqual(result.name, metric.name)
        XCTAssertEqual(result.traceId, metric.traceId)
        XCTAssertEqual(result.spanId, try XCTUnwrap(metric.spanId))
        XCTAssertEqual(result.value.type, .distribution)
        XCTAssertEqual(result.value.distributionValue, 4.25, accuracy: 0.001)
        XCTAssertEqual(result.unit, "millisecond")
        XCTAssertEqual(result.attributes["source"]?.stringValue, "swift")
    }

    func testMetricToSwift_shouldIncludeAllProperties() throws {
        let timestamp = Date(timeIntervalSince1970: 123)
        let traceId = SentryId(uuidString: "12345678123456781234567812345678")
        let spanId = SpanId(value: "8765432112345678")
        let metric = SentryObjCMetric(
            timestamp: timestamp,
            name: "updated.metric",
            trace: traceId,
            spanId: spanId,
            value: SentryObjCMetricValue.gauge(withValue: 5.5),
            unit: "second",
            attributes: [
                "source": SentryObjCAttributeContent.string(withValue: "objc")
            ]
        )

        let result = metric.toSwift()

        XCTAssertEqual(result.timestamp, timestamp)
        XCTAssertEqual(result.name, "updated.metric")
        XCTAssertEqual(result.traceId, traceId)
        XCTAssertEqual(result.spanId, spanId)
        XCTAssertEqual(result.value, .gauge(5.5))
        XCTAssertEqual(result.unit, "second")
        XCTAssertEqual(result.attributes["source"]?.anyValue as? String, "objc")
    }

    func testBeforeSendMetric_whenObjCCallbackMutatesMetric_shouldReturnMutatedSwiftMetric() throws {
        let options = Options()
        let updatedTimestamp = Date(timeIntervalSince1970: 456)
        let updatedTraceId = SentryId(uuidString: "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa")
        let updatedSpanId = SpanId(value: "aaaaaaaaaaaaaaaa")

        // Set the ObjC-typed callback directly via the @objc(setBeforeSendMetric:)
        // setter exposed by Sources/Swift/Options.swift. This replaces the call
        // to the deleted `SentrySwiftBridge.bridgeBeforeSendMetric(forOptions:_:)`.
        options._objcBeforeSendMetric = { metric in
            metric.timestamp = updatedTimestamp
            metric.name = "callback.metric"
            metric.traceId = updatedTraceId
            metric.spanId = updatedSpanId
            metric.value = SentryObjCMetricValue.counter(withValue: 10)
            metric.unit = "item"
            metric.attributes = [
                "source": SentryObjCAttributeContent.string(withValue: "callback")
            ]
            return metric
        }

        let result = try XCTUnwrap(options.beforeSendMetric?(originalMetric()))

        XCTAssertEqual(result.timestamp, updatedTimestamp)
        XCTAssertEqual(result.name, "callback.metric")
        XCTAssertEqual(result.traceId, updatedTraceId)
        XCTAssertEqual(result.spanId, updatedSpanId)
        XCTAssertEqual(result.value, .counter(10))
        XCTAssertEqual(result.unit, "item")
        XCTAssertEqual(result.attributes["source"]?.anyValue as? String, "callback")
    }

    func testBeforeSendMetric_whenObjCCallbackReturnsNil_shouldDropMetric() throws {
        let options = Options()
        options._objcBeforeSendMetric = { _ in nil }

        let result = options.beforeSendMetric?(originalMetric())

        XCTAssertNil(result ?? nil)
    }

    func testBeforeSendMetric_whenSetToNil_shouldClearSwiftCallback() throws {
        let options = Options()
        options._objcBeforeSendMetric = { metric in metric }
        XCTAssertNotNil(options.beforeSendMetric)

        options._objcBeforeSendMetric = nil

        XCTAssertNil(options.beforeSendMetric)
        XCTAssertNil(options._objcBeforeSendMetric)
    }

    private func originalMetric() -> SentryMetric {
        var metric = SentryMetric(
            timestamp: Date(timeIntervalSince1970: 1),
            traceId: SentryId(uuidString: "550e8400e29b41d4a716446655440000"),
            name: "original.metric",
            value: .distribution(4.25),
            unit: "millisecond",
            attributes: [
                "source": .string("swift")
            ]
        )
        metric.spanId = SpanId(value: "b0e6f15b45c36b12")
        return metric
    }
}
