#[repr(C)]
struct Radixtree {
    _private: [u8; 0], // zero-sized type
}

unsafe extern "C" {
    fn radixtree_ffi_init() -> *mut Radixtree;
    fn radixtree_ffi_free(tree: *mut Radixtree);
    fn radixtree_ffi_insert(tree: *mut Radixtree, key: usize, value: usize) -> i32;
    fn radixtree_ffi_delete(tree: *mut Radixtree, key: usize, deleted_value: *mut usize) -> i32;
    fn radixtree_ffi_search(tree: *mut Radixtree, key: usize, found_value: *mut usize) -> i32;
    fn radixtree_ffi_clear(tree: *mut Radixtree);
    fn radixtree_ffi_prune(tree: *mut Radixtree);
}

#[derive(Debug, PartialEq, Eq)]
pub enum RadixtreeStatus {
    Ok,
    Exists,
    NotFound,
    Replaced,
    ErrNoMem,
    ErrInval,
    ErrOverflow,
    ErrUnderflow,
    Unknown,
}

impl RadixtreeStatus {
    pub fn from_errno(errno: i32) -> Self {
        match errno {
            0 => RadixtreeStatus::Ok,
            1 => RadixtreeStatus::Exists,
            2 => RadixtreeStatus::NotFound,
            3 => RadixtreeStatus::Replaced,
            -1 => RadixtreeStatus::ErrNoMem,
            -2 => RadixtreeStatus::ErrInval,
            -3 => RadixtreeStatus::ErrOverflow,
            -4 => RadixtreeStatus::ErrUnderflow,
            _ => RadixtreeStatus::Unknown,
        }
    }
}

impl std::fmt::Display for RadixtreeStatus {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{:?}", self)
    }
}

fn main() {
    println!("Radix Tree FFI Example");

    unsafe {
        let tree = radixtree_ffi_init();

        let mut value: usize = 0;
        let mut ret: i32;
        ret = radixtree_ffi_insert(tree, 0x10, 100);
        println!("Insert 0x10 -> 100: {}", RadixtreeStatus::from_errno(ret));
        ret = radixtree_ffi_insert(tree, 0x20, 200);
        println!("Insert 0x20 -> 200: {}", RadixtreeStatus::from_errno(ret));

        ret = radixtree_ffi_search(tree, 0x10, &mut value as *mut usize);
        println!(
            "Search 0x10: {}, value: {}",
            RadixtreeStatus::from_errno(ret),
            value
        );
        ret = radixtree_ffi_search(tree, 0x20, &mut value as *mut usize);
        println!(
            "Search 0x20: {}, value: {}",
            RadixtreeStatus::from_errno(ret),
            value
        );

        ret = radixtree_ffi_delete(tree, 0x10, &mut value as *mut usize);
        println!(
            "Delete 0x10: {}, deleted value: {}",
            RadixtreeStatus::from_errno(ret),
            value
        );

        ret = radixtree_ffi_search(tree, 0x10, &mut value as *mut usize);
        println!(
            "Search 0x10 after deletion: {}, value: {}",
            RadixtreeStatus::from_errno(ret),
            value
        );
        ret = radixtree_ffi_search(tree, 0x20, &mut value as *mut usize);
        println!(
            "Search 0x20: {}, value: {}",
            RadixtreeStatus::from_errno(ret),
            value
        );

        radixtree_ffi_prune(tree);
        println!("Prune tree");

        radixtree_ffi_clear(tree);
        println!("Clear tree");

        radixtree_ffi_free(tree);
    }
}
