use crate::hal::{
    dma::{
        self, Dma, DmaChannelNumber, DmaCircularMode, DmaMemoryIncrementMode, DmaMemorySize,
        DmaMuxRequest, DmaPeripheralIncrementMode, DmaPeripheralSize, DmaPriorityLevel,
        DmaTransferDirection,
    },
    gpio::{AlternateFunctionMode, Pin, PortName},
    hertz::Hertz,
    nvic::NVIC,
    rcc::RCC,
    timer::{
        Timer, TimerActiveInput, TimerCenterAlignMode, TimerChannel, TimerClockDivision,
        TimerCounterDirection, TimerFilter, TimerPolarity, TimerPrescaler, TimerTriggerOutput,
    },
};
use stm32g0::stm32g031::interrupt;

pub struct IrRx {
    dma_ch: dma::DmaChannel,
    read_index: usize,
}

const DMA_BUFFER_SIZE: usize = 128;
static mut DMA_BUFFER: [u16; DMA_BUFFER_SIZE] = [0; DMA_BUFFER_SIZE];
static mut DMA_CHANNEL: dma::DmaChannelNumber = dma::DmaChannelNumber::Channel5;

static mut TEMP: u16 = 0;

impl IrRx {
    pub fn new(
        mut input_pin: Pin,
        mut timer: Timer,
        mut dma_ch: dma::DmaChannel,
        dma: &mut Dma,
        rcc: &mut RCC,
        nvic: &mut NVIC,
    ) -> IrRx {
        assert_eq!(6, input_pin.get_pin());
        assert_eq!(PortName::A, input_pin.get_port_name());
        assert_eq!(3, timer.get_timer_number());
        assert_eq!(DmaChannelNumber::Channel5, dma_ch.get_channel());

        input_pin.set_as_input();
        input_pin.set_as_alternate_function(AlternateFunctionMode::AF1);

        dma.disable_channel(dma_ch.get_channel());
        dma_ch.set_peripheral_request(DmaMuxRequest::Timer3Channel1);
        dma.set_data_transfer_direction(
            dma_ch.get_channel(),
            DmaTransferDirection::PeripheralToMemory,
        );
        dma.set_channel_priority_level(dma_ch.get_channel(), DmaPriorityLevel::Low);
        dma.set_channel_circular_mode(dma_ch.get_channel(), DmaCircularMode::Circular);
        dma.set_peripheral_increment_mode(
            dma_ch.get_channel(),
            DmaPeripheralIncrementMode::Disabled,
        );

        dma.set_memory_increment_mode(dma_ch.get_channel(), DmaMemoryIncrementMode::Increment);
        dma.set_peripheral_size(dma_ch.get_channel(), DmaPeripheralSize::U16);
        dma.set_memory_size(dma_ch.get_channel(), DmaMemorySize::U16);
        dma.clear_global_interrupt_flag(dma_ch.get_channel());
        dma.set_data_length(dma_ch.get_channel(), DMA_BUFFER_SIZE as u16);
        dma.set_peripheral_address(
            dma_ch.get_channel(),
            timer.get_capture_compare_register_address(),
        );
        dma.set_memory_address(dma_ch.get_channel(), unsafe { DMA_BUFFER.as_mut_ptr() }
            as u32);

        timer.set_counter_direction(TimerCounterDirection::Up);
        timer.set_center_align_mode(TimerCenterAlignMode::Edge);
        timer.set_clock_division(TimerClockDivision::Div1);
        timer.set_auto_reload(65535);

        timer.disable_auto_reload_preload();
        timer.set_trigger_output(TimerTriggerOutput::Reset);
        timer.disable_master_slave_mode();
        timer.ic_set_active_input(TimerChannel::Channel1, TimerActiveInput::DirectTI);
        timer.ic_set_prescaler(TimerChannel::Channel1, TimerPrescaler::Div1);
        timer.ic_set_filter(TimerChannel::Channel1, TimerFilter::Div1);
        timer.ic_set_polarity(TimerChannel::Channel1, TimerPolarity::BothEdges);

        timer.set_prescaler_hertz(Hertz::megahertz(1), &rcc);
        timer.generate_event_update();

        dma.enable_interrupt_transfer_complete(dma_ch.get_channel());
        dma.enable_interrupt_transfer_error(dma_ch.get_channel());

        // start dma
        dma.enable_channel(dma_ch.get_channel());

        let read_index = get_dma_rx_pos(&dma, &dma_ch);

        timer.enable_capture_compare_interrupt(TimerChannel::Channel1);
        timer.enable_capture_compare_dma_request(TimerChannel::Channel1);
        timer.enable_capture_compare_channel(TimerChannel::Channel1);
        timer.enable_counter();

        // TODO do we need this?
        nvic.enable_interrupt_timer3();

        unsafe {
            DMA_CHANNEL = dma_ch.get_channel();
        }

        return IrRx { dma_ch, read_index };
    }

    pub fn read(&mut self, dma: &Dma) -> Option<u16> {
        let dma_index = get_dma_rx_pos(&dma, &self.dma_ch);
        let mut read_index = self.read_index;
        if dma_index != read_index {
            let result = unsafe { DMA_BUFFER[read_index] };
            read_index = read_index + 1;
            if read_index >= DMA_BUFFER_SIZE {
                read_index = 0;
            }
            self.read_index = read_index;
            return Option::Some(result);
        }
        return Option::None;
    }
}

fn get_dma_rx_pos(dma: &Dma, dma_ch: &dma::DmaChannel) -> usize {
    let v = dma.get_data_length(dma_ch.get_channel()) as usize;
    return DMA_BUFFER_SIZE - v;
}

#[interrupt]
fn ADC_COMP() {
    unsafe {
        TEMP = TEMP + 1;
    }
}

#[interrupt]
fn TIM1_CC() {
    unsafe {
        TEMP = TEMP + 1;
    }
}

#[interrupt]
fn TIM2() {
    unsafe {
        TEMP = TEMP + 1;
    }
}

#[interrupt]
fn TIM3() {
    unsafe {
        TEMP = TEMP + 1;
    }
}

#[interrupt]
fn DMA_CHANNEL1() {
    unsafe {
        TEMP = TEMP + 1;
    }
    let ch = unsafe { DMA_CHANNEL };
    if Dma::is_global_interrupt_flag_set(ch) {
        unsafe {
            TEMP = TEMP + 1;
        }
    }
    if Dma::is_transfer_complete_interrupt_flag_set(ch) {
        unsafe {
            TEMP = TEMP + 1;
        }
    }
}

#[interrupt]
fn DMA_CHANNEL2_3() {
    unsafe {
        TEMP = TEMP + 1;
    }
    let ch = unsafe { DMA_CHANNEL };
    if Dma::is_global_interrupt_flag_set(ch) {
        unsafe {
            TEMP = TEMP + 1;
        }
    }
    if Dma::is_transfer_complete_interrupt_flag_set(ch) {
        unsafe {
            TEMP = TEMP + 1;
        }
    }
}

#[interrupt]
fn DMA_CHANNEL4_5_6_7() {
    unsafe {
        TEMP = TEMP + 1;
    }
    let ch = unsafe { DMA_CHANNEL };
    if Dma::is_global_interrupt_flag_set(ch) {
        unsafe {
            TEMP = TEMP + 1;
        }
    }
    if Dma::is_transfer_complete_interrupt_flag_set(ch) {
        unsafe {
            TEMP = TEMP + 1;
        }
    }
}
