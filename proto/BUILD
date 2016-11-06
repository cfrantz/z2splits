package(default_visibility=["//visibility:public"])
load("@google_protobuf//:protobuf.bzl", "cc_proto_library")

cc_proto_library(
    name = "controller",
    srcs = ["controller.proto"],
    default_runtime = "@google_protobuf//:protobuf",
    protoc = "@google_protobuf//:protoc",
)

cc_proto_library(
    name = "config",
    srcs = [
        "config.proto"
    ],
    deps = [
        ":controller",
    ],
    default_runtime = "@google_protobuf//:protobuf",
    protoc = "@google_protobuf//:protoc",
)

# vim: ts=4 sw=4 sts=4 expandtab:
