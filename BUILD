package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "z2splits",
    data = ["//content"],
    linkopts = [
        "-lSDL2",
        "-lSDL2_image",
        "-lSDL2_mixer",
    ],
    srcs = ["main.cc"],
    deps = [
        ":config",
        ":splits_screen",
        ":remote",
        "@libgam//:game",
        "//external:gflags",
    ],
)

cc_library(
    name = "splits_screen",
    srcs = ["splits_screen.cc"],
    hdrs = ["splits_screen.h"],
    deps = [
        "//proto:config",
        "//util:file",
        "//util:logging",
        "//util:os",
        "//util:string",
        "@libgam//:screen",
        "@libgam//:spritemap",
        "@libgam//:text",
    ],
)


cc_library(
    name = "config",
    srcs = ["config.cc"],
    hdrs = ["config.h"],
    deps = [
        "//util:file",
        "//util:logging",
        "//util:os",
        "//proto:config",
    ],
)

cc_library(
    name = "remote",
    srcs = ["remote.cc"],
    hdrs = ["remote.h"],
    deps = [
        ":splits_screen",
        "//util:file",
        "//util:logging",
        "//proto:remote",
    ],
)
