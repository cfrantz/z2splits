git_repository(
    name = "libgam",
    remote = "https://github.com/bentglasstube/gam.git",
    commit = "44411dac629b8bbbd1c966d8509b7f054003e3dd",
)

######################################################################
# gflags
######################################################################
git_repository(
    name = "gflags_git",
    #commit = "HEAD",  # Use the current HEAD commit
    commit = "74bcd20c0e5b904a67e37abf0c1262824ff9030c",
    remote = "https://github.com/gflags/gflags.git",
)

bind(
    name = "gflags",
    actual = "@gflags_git//:gflags",
)

######################################################################
# protobuf
######################################################################
git_repository(
	name = "google_protobuf",
	remote = "https://github.com/google/protobuf.git",
	tag = "v3.1.0"
)
