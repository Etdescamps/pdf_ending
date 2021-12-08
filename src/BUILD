cc_library(
    name = "file_map",
    srcs = ["file_map.cc"],
    hdrs = ["file_map.hh"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "pdf_end",
    srcs = ["pdf_end.cc"],
    deps = [":file_map"],
)
