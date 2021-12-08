cc_library(
    name = "random_file",
    srcs = ["random_file.cc"],
    hdrs = ["random_file.hh"],
    deps = ["//src:file_map"],
)

cc_binary(
    name = "file_generator",
    srcs = ["file_generator.cc"],
    deps = [":random_file"],
)

