load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "tast",
    srcs = ["tast.c"],
    linkopts = ["/SUBSYSTEM:WINDOWS"],
)
cc_library(
    name = "FileUtils",
    srcs = ["FileUtils.c",
            "FileUtils.h"],
    hdrs = ["FileUtils.h"],
)

genrule(
    name = "res",
    outs = ["3vs3.res"],
    cmd = "cp $(rootpath //src:3vs3.res) $(execpath //src:3vs3.res)"
)

cc_binary(
    name = "main",
    srcs = ["main.c",
            "main.h"],
    deps = [":FileUtils"],
    data = [":res"], # Ensure this is correctly handled if you need it at runtime, not for linking
    additional_linker_inputs = [":res"], # This correctly adds the .res file to the linker's inputs
     linkopts = ["/SUBSYSTEM:WINDOWS",
                "bazel-out/x64_windows-fastbuild/bin/src/3vs3.res"
    ],
)
