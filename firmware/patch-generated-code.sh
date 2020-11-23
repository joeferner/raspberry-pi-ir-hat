#!/bin/bash
set -e

DIR=$(cd "$(dirname "$0")" && pwd)
cd "${DIR}"

if ! grep -q 'extern void setup\(\)' Core/Src/main.c; then
  echo "patching Core/Src/main.c: adding declarations"
  sed -i -e 's|/\* USER CODE BEGIN 0 \*/|/* USER CODE BEGIN 0 */\nextern void setup();\nextern void loop();\n|' Core/Src/main.c
fi

if ! grep -q 'setup\(\)' Core/Src/main.c; then
  echo "patching Core/Src/main.c: adding setup()"
  sed -i -e 's|/\* USER CODE BEGIN 2 \*/|/* USER CODE BEGIN 2 */\n  setup();\n|' Core/Src/main.c
fi

if ! grep -q 'loop\(\)' Core/Src/main.c; then
  echo "patching Core/Src/main.c: adding loop()"
  sed -i -e 's|/\* USER CODE END WHILE \*/|/* USER CODE END WHILE */\n    loop();\n|' Core/Src/main.c
fi

for f in Core/Src/stm32g0xx_it.c; do
  if [ $(wc -l "$f" | awk '{print $1}') -gt 1 ]; then
    echo "clearing $f"
    echo "" > "$f"
  fi
done

echo "complete"
