// swift-tools-version:5.3

import PackageDescription

let package = Package(
    name: "TreeSitterLua",
    platforms: [.macOS(.v10_13), .iOS(.v11)],
    products: [
        .library(name: "TreeSitterLua", targets: ["TreeSitterLua"]),
    ],
    dependencies: [],
    targets: [
        .target(name: "TreeSitterLua",
                path: ".",
                exclude: [
                    "binding.gyp",
                    "bindings",
                    "Cargo.toml",
                    "grammar.js",
                    "LICENSE.txt",
                    "package.json",
                    "README.md",
                ],
                sources: [
                    "src/parser.c",
                    "src/scanner.c",
                ],
                publicHeadersPath: "bindings/swift",
                cSettings: [.headerSearchPath("src")])
    ]
)
