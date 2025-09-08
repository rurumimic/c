fn main() {
    cc::Build::new()
        .include("libs/radixtree/include")
        .file("libs/radixtree/src/radixtree_ffi.c")
        .file("libs/radixtree/src/radixtree_node.c")
        .file("libs/radixtree/src/radixtree.c")
        .file("libs/radixtree/src/rdx_alloc.c")
        .flag_if_supported("-std=c11")
        .flag_if_supported("-O3")
        .flag_if_supported("-fvisibility=hidden")
        .warnings(true)
        .extra_warnings(true)
        .compile("radixtree");

    for path in [
        "cargo:rerun-if-changed=libs/radixtree/include/radixtree_status.h",
        "cargo:rerun-if-changed=libs/radixtree/include/radixtree_ffi.h",
        "cargo:rerun-if-changed=libs/radixtree/include/radixtree.h",
        "cargo:rerun-if-changed=libs/radixtree/src/radixtree_ffi.c",
        "cargo:rerun-if-changed=libs/radixtree/src/radixtree_node.c",
        "cargo:rerun-if-changed=libs/radixtree/src/radixtree.c",
        "cargo:rerun-if-changed=libs/radixtree/src/rdx_alloc.c",
    ] {
        println!("cargo:rerun-if-changed={path}");
    }
}
