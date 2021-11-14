build:
	cargo build --release --target thumbv6m-none-eabi
	arm-none-eabi-objcopy --output-target binary target/thumbv6m-none-eabi/release/firmware target/thumbv6m-none-eabi/release/firmware.bin
	./print-memory-usage.sh target/thumbv6m-none-eabi/release/firmware

flash: build
	./flash.sh
