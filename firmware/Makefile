build:
	cargo build --release
	arm-none-eabi-objcopy --output-target binary target/thumbv6m-none-eabi/release/firmware target/thumbv6m-none-eabi/release/firmware.bin
	./print-memory-usage.sh target/thumbv6m-none-eabi/release/firmware

flash: build
	./flash.sh

print-largest-fn: build
	nm --print-size --size-sort --radix=d target/thumbv6m-none-eabi/release/firmware | grep -v ' b '
