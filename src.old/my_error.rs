pub type Result<T> = std::result::Result<T, MyError>;
use thiserror::Error;

#[derive(Error, Debug)]
pub enum MyError {
    #[error("spi error: {0}")]
    SpiError(#[from] rppal::spi::Error),
    #[error("std io error: {0}")]
    StdIoError(#[from] std::io::Error),
    #[error("nix error: {0}")]
    NixErrnoError(#[from] nix::errno::Errno),
    #[error("try from slice error: {0}")]
    TryFromSliceError(#[from] std::array::TryFromSliceError),
    #[error("gpio error: {0}")]
    GpioError(#[from] rppal::gpio::Error),
    #[error("try from int error: {0}")]
    TryFromIntError(#[from] std::num::TryFromIntError),
    #[error("receive error: {0}")]
    RecvError(#[from] std::sync::mpsc::RecvError),
    #[error("system time error: {0}")]
    SystemTimeError(#[from] std::time::SystemTimeError),
    #[error("generic error: {0}")]
    GenericError(String),
}

impl MyError {
    pub fn new<S: Into<String>>(message: S) -> Self {
        return MyError::GenericError(message.into());
    }
}
