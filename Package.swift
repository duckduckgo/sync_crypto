// swift-tools-version: 5.7
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "DDGSyncCrypto",
    platforms: [
        .iOS(.v14),
        .macOS(.v10_15)
    ],
    products: [
        .library(
            name: "DDGSyncCrypto",
            targets: ["DDGSyncCrypto"]
        ),
    ],
    targets: [
        .binaryTarget(
            name: "Clibsodium",
            path: "native_lib/third-party/apple/Clibsodium.xcframework"
        ),
        .target(
            name: "DDGSyncCrypto",
            dependencies: ["Clibsodium"],
            path: "native_lib",
            exclude: ["native_lib/third-party"],
            publicHeadersPath: "."
        )
    ]
)
