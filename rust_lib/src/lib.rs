//! Blinks the LED on a Pico board
//!
//! This will blink an LED attached to GP25, which is the pin the Pico uses for the on-board LED.
#![no_std]
#![no_main]

// use bsp::entry;
// use defmt::*;
// use defmt_rtt as _;
use embedded_hal::digital::v2::OutputPin;
// use embedded_time::fixed_point::FixedPoint;
// use panic_probe as _;

#[panic_handler]
fn panic(_panic: &core::panic::PanicInfo<'_>) -> ! {
    loop {}
}

// Provide an alias for our BSP so we can switch targets quickly.
// Uncomment the BSP you included in Cargo.toml, the rest of the code does not need to change.
use rp_pico as bsp;
// use sparkfun_pro_micro_rp2040 as bsp;

use bsp::hal::{
    //     clocks::{init_clocks_and_plls, Clock},
    pac,
    sio::Sio,
    //     watchdog::Watchdog,
};

use cty::c_int;

#[no_mangle]
pub extern "C" fn rust_function() -> c_int {
    123
}

#[no_mangle]
pub extern "C" fn rust_led_put(value: bool) {
    let mut pac = pac::Peripherals::take().unwrap();
    let sio = Sio::new(pac.SIO);

    let pins = bsp::Pins::new(
        pac.IO_BANK0,
        pac.PADS_BANK0,
        sio.gpio_bank0,
        &mut pac.RESETS,
    );

    let mut led_pin = pins.led.into_push_pull_output();

    if value {
        led_pin.set_high().unwrap();
    } else {
        led_pin.set_low().unwrap();
    }
}

// End of file
