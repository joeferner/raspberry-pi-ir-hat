build:
	cargo build --release --target thumbv6m-none-eabi
	arm-none-eabi-objcopy --output-target binary target/thumbv6m-none-eabi/release/firmware target/thumbv6m-none-eabi/release/firmware.bin
	arm-none-eabi-objdump -d --line-numbers target/thumbv6m-none-eabi/release/firmware > target/thumbv6m-none-eabi/release/firmware.dump
	./print-memory-usage.sh target/thumbv6m-none-eabi/release/firmware

flash: build
	./flash.sh

test:
	cargo test

debug:
	cargo run --release

openocd:
	openocd -f interface/stlink.cfg -f stm32g0x.cfg -c "init; reset halt"

clean:
	rm -rf target