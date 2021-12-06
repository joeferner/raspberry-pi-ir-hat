use core::usize;

use core::str;
use core::str::Bytes;
use heapless::Deque;

pub enum BufferedIoError {}

#[cfg(test)]
impl std::fmt::Debug for BufferedIoError {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(f, "{:?}", self)
    }
}

pub enum PeekUntilResult {
    Found(usize),
    EndOfFifo(usize),
    NotFound,
}

pub struct BufferedIo<'a, const LEN: usize> {
    rx_fifo: Deque<u8, LEN>,
    target: &'a mut dyn BufferedIoTarget,
}

impl<'a, const LEN: usize> BufferedIo<'a, LEN> {
    pub fn new(target: &'a mut dyn BufferedIoTarget) -> Self {
        return BufferedIo {
            rx_fifo: Deque::new(),
            target,
        };
    }

    pub fn tick(&mut self) {
        self.fill_rx_fifo();
    }

    pub fn write_str(&mut self, value: &str) -> Result<(), BufferedIoError> {
        return self.target.write_bytes(&mut value.bytes());
    }

    pub fn write(&mut self, value: u8) -> Result<(), BufferedIoError> {
        return self.target.write(value);
    }

    pub fn find(&mut self, stop: u8) -> PeekUntilResult {
        self.fill_rx_fifo();

        let mut read = 0;
        for b in self.rx_fifo.iter() {
            read = read + 1;
            if *b == stop {
                return PeekUntilResult::Found(read);
            }
        }
        if read == self.rx_fifo.len() {
            return PeekUntilResult::EndOfFifo(read);
        } else {
            return PeekUntilResult::NotFound;
        }
    }

    pub fn read_line<'b>(&mut self, buf: &'b mut [u8]) -> Result<Option<&'b str>, BufferedIoError> {
        let len_to_read = match self.find(b'\n') {
            PeekUntilResult::Found(offset) => offset,
            PeekUntilResult::NotFound => {
                return Result::Ok(Option::None);
            }
            PeekUntilResult::EndOfFifo(offset) => offset,
        };
        let l = self.read_bytes(len_to_read, buf);
        unsafe {
            let s = str::from_utf8_unchecked(&buf[0..l]);
            return Result::Ok(Option::Some(s));
        }
    }

    pub fn read_bytes(&mut self, len: usize, buffer: &mut [u8]) -> usize {
        for i in 0..len {
            if let Option::Some(v) = self.rx_fifo.pop_front() {
                buffer[i] = v;
            } else {
                return i;
            }
        }
        return len;
    }

    fn fill_rx_fifo(&mut self) {
        while !self.rx_fifo.is_full() {
            match self.target.read() {
                Option::None => {
                    return;
                }
                Option::Some(v) => {
                    self.rx_fifo.push_back(v).unwrap();
                }
            }
        }
    }
}

pub trait BufferedIoTarget {
    fn write_bytes(&mut self, value: &mut Bytes) -> Result<(), BufferedIoError>;

    fn write(&mut self, value: u8) -> Result<(), BufferedIoError>;

    fn read(&mut self) -> Option<u8>;
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_read_line() {
        let mut buf = [0u8; 100];
        let mut target = MockReadTarget::new("test\n");
        let mut io: BufferedIo<100> = BufferedIo::new(&mut target);
        let read_line_results = io.read_line(&mut buf);
        let line = read_line_results.unwrap();
        assert_eq!("test\n", line.unwrap());
    }

    #[test]
    fn test_read_line_line_does_not_fit_in_buffer() {
        let mut buf = [0u8; 3];
        let mut target = MockReadTarget::new("test\n");
        let mut io: BufferedIo<100> = BufferedIo::new(&mut target);
        let read_line_results = io.read_line(&mut buf);
        let line = read_line_results.unwrap();
        assert_eq!("test", line.unwrap());

        let read_line_results = io.read_line(&mut buf);
        let line = read_line_results.unwrap();
        assert_eq!("t\n", line.unwrap());
    }

    struct MockReadTarget {
        data: Vec<u8>,
    }

    impl MockReadTarget {
        pub fn new(s: &str) -> Self {
            let mut data = Vec::new();
            for b in s.as_bytes() {
                data.push(*b);
            }
            return MockReadTarget { data };
        }
    }

    impl BufferedIoTarget for MockReadTarget {
        fn write_bytes(&mut self, _value: &mut Bytes) -> Result<(), BufferedIoError> {
            todo!()
        }

        fn write(&mut self, _value: u8) -> Result<(), BufferedIoError> {
            todo!()
        }

        fn read(&mut self) -> Option<u8> {
            return self.data.pop();
        }
    }
}
