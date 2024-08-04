#![no_std]

use core::panic::PanicInfo;

#[panic_handler]
fn panik(_panic_info: &PanicInfo) -> ! {
    loop {}
}

extern "C" {
    // C function declarations go here
    // extern C functions are by nature unsafe
    // fn printf(format: *const u8, ...);

}

#[no_mangle]
pub extern "C" fn analyze(value: f32) -> u16 {
    match value {
        0.0..20.0 => 0,
        20.0..30.0 => 1,
        30.0..40.0 => 2,
        40.0..50.0 => 3,
        50.0..70.0 => 4,
        70.0..100.0 => 5,
        _ => 5,
    }
}
