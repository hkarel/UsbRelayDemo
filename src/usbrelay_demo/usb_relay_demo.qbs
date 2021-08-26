import qbs
import QbsUtl

Product {
    name: "UsbRelay (Demo)"
    targetName: "usb-relay-demo"
    condition: true

    type: "application"
    destinationDirectory: "./bin"

    Depends { name: "cpp" }
    Depends { name: "SharedLib" }
    Depends { name: "UsbRelay" }
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

    cpp.dynamicLibraries: [
        "pthread",
        "usb-1.0",
    ]

    Group {
        name: "resources"
        files: [
            "usb_relay_demo.qrc",
        ]
    }

    files: [
        "main_window.cpp",
        "main_window.h",
        "main_window.ui",
        "usb_relay_demo.cpp",
    ]

//    property var test: {
//        console.info("=== Video.decklinkIncludePath ===");
//        console.info(project.decklinkIncludePath);
//    }

}
