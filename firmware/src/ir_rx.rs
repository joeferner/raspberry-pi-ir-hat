use crate::hal::{
    dma::{
        self, Dma, DmaChannelNumber, DmaCircularMode, DmaMemoryIncrementMode, DmaMemorySize,
        DmaMuxRequest, DmaPeripheralIncrementMode, DmaPeripheralSize, DmaPriorityLevel,
        DmaTransferDirection,
    },
    gpio::gpioa,
    gpio::AlternateFunctionMode,
    hertz::Hertz,
    nvic::NVIC,
    rcc::RCC,
    timer::{
        Timer, TimerActiveInput, TimerCenterAlignMode, TimerChannel, TimerClockDivision,
        TimerCounterDirection, TimerFilter, TimerNumber, TimerPolarity, TimerPrescaler,
        TimerTriggerOutput,
    },
};

pub struct IrRx {
    dma_ch: dma::DmaChannel,
    read_index: usize,
    last_value: u16,
}

const DMA_BUFFER_SIZE: usize = 128;
static mut DMA_BUFFER: [u16; DMA_BUFFER_SIZE] = [0; DMA_BUFFER_SIZE];
static mut DMA_CHANNEL: dma::DmaChannelNumber = dma::DmaChannelNumber::Channel5;

impl IrRx {
    pub fn new(
        mut input_pin: gpioa::PA6,
        mut timer: Timer,
        timer_channel: TimerChannel,
        mut dma_ch: dma::DmaChannel,
        dma: &mut Dma,
        rcc: &mut RCC,
        _nvic: &mut NVIC,
    ) -> IrRx {
        debug_assert_eq!(TimerNumber::Timer3, timer.get_timer_number());
        debug_assert_eq!(TimerChannel::Channel1, timer_channel);
        debug_assert_eq!(DmaChannelNumber::Channel5, dma_ch.get_channel());

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
        timer.ic_set_active_input(timer_channel, TimerActiveInput::DirectTI);
        timer.ic_set_prescaler(timer_channel, TimerPrescaler::Div1);
        timer.ic_set_filter(timer_channel, TimerFilter::Div1);
        timer.ic_set_polarity(timer_channel, TimerPolarity::BothEdges);

        timer.set_prescaler_hertz(Hertz::megahertz(1), &rcc);
        timer.generate_event_update();

        // start dma
        dma.enable_channel(dma_ch.get_channel());

        let read_index = IrRx::get_dma_rx_pos(&dma, &dma_ch);

        timer.enable_capture_compare_dma_request(timer_channel);
        timer.enable_capture_compare_channel(timer_channel);
        timer.enable_counter();

        unsafe {
            DMA_CHANNEL = dma_ch.get_channel();
        }

        return IrRx {
            dma_ch,
            read_index,
            last_value: 0,
        };
    }

    pub fn read(&mut self, dma: &Dma) -> Option<u16> {
        let dma_index = IrRx::get_dma_rx_pos(&dma, &self.dma_ch);
        let mut read_index = self.read_index;
        if dma_index != read_index {
            let v = unsafe { DMA_BUFFER[read_index] };
            read_index = read_index + 1;
            if read_index >= DMA_BUFFER_SIZE {
                read_index = 0;
            }
            self.read_index = read_index;

            let result = v - self.last_value;
            self.last_value = v;
            return Option::Some(result);
        }
        return Option::None;
    }

    fn get_dma_rx_pos(dma: &Dma, dma_ch: &dma::DmaChannel) -> usize {
        let v = dma.get_data_length(dma_ch.get_channel()) as usize;
        return DMA_BUFFER_SIZE - v;
    }
}
