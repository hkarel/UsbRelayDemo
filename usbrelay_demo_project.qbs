import qbs
import qbs.File
import "qbs/imports/QbsUtl/qbsutl.js" as QbsUtl

Project {
    name: "UsbRelayDemo (Project)"

    minimumQbsVersion: "1.22.0"
    qbsSearchPaths: ["qbs"]

    readonly property var projectVersion: projectProbe.projectVersion
    readonly property string projectGitRevision: projectProbe.projectGitRevision

    Probe {
        id: projectProbe
        property var projectVersion;
        property string projectGitRevision;

        readonly property string projectBuildDirectory:  project.buildDirectory
        readonly property string projectSourceDirectory: project.sourceDirectory

        configure: {
            projectVersion = QbsUtl.getVersions(projectSourceDirectory + "/VERSION");
            projectGitRevision = QbsUtl.gitRevision(projectSourceDirectory);
            if (File.exists(projectBuildDirectory + "/package_build_info"))
                File.remove(projectBuildDirectory + "/package_build_info")
        }
    }

    property var cppDefines: {
        var def = [
            "VERSION_PROJECT=\"" + projectVersion[0] + "\"",
            "VERSION_PROJECT_MAJOR=" + projectVersion[1],
            "VERSION_PROJECT_MINOR=" + projectVersion[2],
            "VERSION_PROJECT_PATCH=" + projectVersion[3],
            "GIT_REVISION=\"" + projectGitRevision + "\"",
            "LOGGER_LESS_SNPRINTF",
        ];

        if (qbs.buildVariant === "release")
            def.push("NDEBUG");

        def.push("CONFIG_DIR=\"/etc/usbrelay-demo\"");
        return def;
    }

    property var cxxFlags: [
        "-ggdb3",
        "-Wall",
        "-Wextra",
        "-Wswitch-enum",
        "-Wdangling-else",
        "-Wno-unused-parameter",
        "-Wno-variadic-macros",
        "-Wno-vla",
    ]
    property string cxxLanguageVersion: "c++17"

    references: [
        "src/shared/shared.qbs",
        "src/usbrelay/usb_relay.qbs",
        "src/usbrelay_demo/usbrelay_demo.qbs",
        "src/yaml/yaml.qbs",
    ]
}
