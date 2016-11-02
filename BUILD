package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "splits",
    data = ["//content"],
    linkopts = [
        "-lSDL2",
        "-lSDL2_image",
        "-lSDL2_mixer",
    ],
    srcs = ["main.cc"],
    deps = [
        ":splits_screen",
        "@libgam//:game",
    ],
)

cc_library(
    name = "splits_screen",
    srcs = ["splits_screen.cc"],
    hdrs = ["splits_screen.h"],
    deps = [
        "@libgam//:screen",
        "@libgam//:text",
    ],
)
