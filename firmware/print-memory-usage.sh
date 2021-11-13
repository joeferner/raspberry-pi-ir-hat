#!/bin/bash

if [ $# -le 0 ]; then
    echo "This script requires 1 arguments."
    echo -e "\nUsage:\nget-fw-size FILE\n"
    exit 1
fi

file=$1
max_flash=$(cat memory.x | grep 'FLASH.*ORIGIN.*LENGTH' | sed -n 's|^.*LENGTH = \([^=]*\)K$|\1|p')
max_ram=$(cat memory.x | grep 'RAM.*ORIGIN.*LENGTH' | sed -n 's|^.*LENGTH = \([^=]*\)K$|\1|p')

function print_region() {
    size=$1
    max_size=$2
    name=$3

    if [[ $max_size == 0x* ]]; then
        max_size=$(echo ${max_size:2})
        max_size=$(( 16#$max_size ))
    fi

    max_size=$(( max_size * 1024 ))
    pct=$(( 100 * $size / $max_size ))
    echo "$name used: $size / $max_size ($pct%)"
}

raw=$(arm-none-eabi-size $file)

text=$(echo $raw | cut -d ' ' -f 7)
data=$(echo $raw | cut -d ' ' -f 8)
bss=$(echo $raw | cut -d ' ' -f 9)

flash=$(($text + $data))
ram=$(($data + $bss))

echo ""
print_region $flash $max_flash "Flash"
print_region $ram $max_ram "  RAM"
echo ""
