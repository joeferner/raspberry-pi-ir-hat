#!/bin/bash
set -e

DIR=$(cd "$(dirname "$0")" && pwd)
cd "${DIR}"

dos2unix Makefile

if ! grep -q 'extern void setup\(\)' Core/Src/main.c; then
  echo "patching Core/Src/main.c: adding declarations"
  sed -i -e 's|/\* USER CODE BEGIN 0 \*/|/* USER CODE BEGIN 0 */\nextern void setup();\nextern void loop();\n|' Core/Src/main.c
fi

if ! (cat Core/Src/main.c | grep -v extern | grep -qE 'setup\(\)'); then
  echo "patching Core/Src/main.c: adding setup()"
  sed -i -e 's|/\* USER CODE BEGIN 2 \*/|/* USER CODE BEGIN 2 */\n  setup();\n|' Core/Src/main.c
fi

if ! (cat Core/Src/main.c | grep -v extern | grep -qE 'loop\(\)'); then
  echo "patching Core/Src/main.c: adding loop()"
  sed -i -e 's|/\* USER CODE END WHILE \*/|loop();\n    /* USER CODE END WHILE */|' Core/Src/main.c
fi

if cat Core/Src/main.c | grep -q 'LL_IWDG_Enable'; then
  echo "patching Core/Src/main.c: removing LL_IWDG_Enable"
  sed -i -e 's|.*LL_IWDG_Enable.*||' Core/Src/main.c
fi

for f in Core/Src/stm32g0xx_it.c; do
  if [ $(wc -l "$f" | awk '{print $1}') -gt 1 ]; then
    echo "clearing $f"
    echo "" > "$f"
  fi
done

echo "#ifndef _config_generated_h_" > Core/Inc/config_generated.h
echo "#define _config_generated_h_" >> Core/Inc/config_generated.h

echo "#define GIT_COMMIT \"$(git rev-parse HEAD)\"" >> Core/Inc/config_generated.h

# ports and pins
for pin_line in $(grep 'Signal=\|GPIO_Label=' firmware.ioc | grep '^P[ABCD]'); do
  port=$(echo "${pin_line}" | sed -e 's|P\([ABCD]\).*|\1|')
  pin=$(echo "${pin_line}" | sed -e 's|P[ABCD]\([0-9]*\).*|\1|')
  name=$(echo "${pin_line}" | sed -e 's|.*=\(.*\)|\1|')
  echo "#define ${name}_PORT GPIO${port}" >> Core/Inc/config_generated.h
  echo "#define ${name}_PIN LL_GPIO_PIN_${pin}" >> Core/Inc/config_generated.h
done

echo "#endif" >> Core/Inc/config_generated.h

echo "complete"
