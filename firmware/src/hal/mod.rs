pub mod baud_rate;
pub mod gpio;
pub mod hertz;
pub mod rcc;
pub mod usart;

pub fn init_1ms_tick(syst: &mut cortex_m::peripheral::SYST) {
    init_tick(syst, 16_000_000, 1000);
}

pub fn init_tick(syst: &mut cortex_m::peripheral::SYST, hclk_frequency: u32, ticks: u32) {
    syst.set_reload((hclk_frequency / ticks) - 1);
    syst.clear_current();
    syst.enable_counter();
}
