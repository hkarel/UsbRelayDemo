import qbs
import QbsUtl

Product {
    name: "UsbRelay"
    targetName: "usb-relay-demo"
    condition: true

    type: "application"
    destinationDirectory: "./bin"

    Depends { name: "cpp" }
    Depends { name: "SharedLib" }
    Depends { name: "Yaml" }
    Depends { name: "Qt"; submodules: ["core", "widgets"] }

    cpp.defines: project.cppDefines
    cpp.cxxFlags: project.cxxFlags
    cpp.cxxLanguageVersion: project.cxxLanguageVersion

    cpp.includePaths: [
        "./",
    ]

    cpp.systemIncludePaths: QbsUtl.concatPaths(
        Qt.core.cpp.includePaths // Декларация для подавления Qt warning-ов
    )

//    Group {
//        name: "resources"
//        files: [
//            "sizterm.qrc",
//        ]
//    }

    files: [
        "main_window.cpp",
        "main_window.h",
        "main_window.ui",
        "usb_relay_demo.cpp",
        "usb_relay_demo.qrc",
    ]

//    property var test: {
//        console.info("=== Video.decklinkIncludePath ===");
//        console.info(project.decklinkIncludePath);
//    }


} // Product
