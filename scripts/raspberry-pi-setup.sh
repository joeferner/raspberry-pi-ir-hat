#!/bin/bash
set -eou pipefail
DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd "${DIR}/.."

IR_IN_PIN=${IR_IN_PIN:-17}
IR_OUT_PIN=${IR_OUT_PIN:-18}

function update {
  sudo apt -y update
  sudo apt -y upgrade
  sudo apt-get autoremove
}

function dev_setup {
  echo "setup dev..."

  git config --global alias.co checkout
  git config --global alias.br branch
  git config --global alias.ci commit
  git config --global alias.st status

  if ! grep EDITOR ~/.bashrc; then
    echo "export EDITOR=/usr/bin/vi" >> ~/.bashrc
  fi
  if ! grep VISUAL ~/.bashrc; then
    echo "export VISUAL=/usr/bin/vi" >> ~/.bashrc
  fi

  sudo adduser "${USER}" spi || echo "already a user"

  echo "dev setup complete"
}

function rust_setup {
  echo "setup rust..."
  if [ ! -f ~/.cargo/bin/rustup ]; then
    curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
  fi
  echo "rust setup complete"
}

function adc_setup {
  echo "setup ADC..."

  # Update /boot/firmware/config.txt
  echo "update /boot/firmware/config.txt"
  sudo sed -i 's|.*dtparam=spi.*|dtparam=spi=on|' /boot/firmware/config.txt
  if ! grep '^dtparam=spi=on$' /boot/firmware/config.txt; then
    echo "failed to update spi"
    exit 1
  fi

  echo "ADC setup complete"
}

function ir_setup {
  echo "setup ir..."

  # Install ir-keytable
  if dpkg -l | grep -q ir-keytable; then
    echo "ir-keytable already installed"
  else
    echo "installing ir-keytable"
    sudo apt install -y ir-keytable
  fi

  # Install evtest
  if dpkg -l | grep -q evtest; then
    echo "evtest already installed"
  else
    echo "installing evtest"
    sudo apt install -y evtest
  fi

  # create /sbin/lirc-allow-write-protocol
  echo "create /sbin/lirc-allow-write-protocol"
  cat << EOF | sudo tee /sbin/lirc-allow-write-protocol
  #!/bin/bash

  chmod a+w /sys/class/rc/rc*/protocols
EOF
  sudo chmod u+x /sbin/lirc-allow-write-protocol

  # create /etc/udev/rules.d/50-lirc-protocol.rules
  echo "/etc/udev/rules.d/50-lirc-protocol.rules"
  cat << EOF | sudo tee /etc/udev/rules.d/50-lirc-protocol.rules
  SUBSYSTEM=="lirc", GROUP="lirc", RUN+="/sbin/lirc-allow-write-protocol"
EOF

  # Update /boot/firmware/config.txt
  echo "update /boot/firmware/config.txt"
  if grep 'gpio-ir,' /boot/firmware/config.txt; then
    sudo sed -i 's|.*gpio-ir,.*|dtoverlay=gpio-ir,gpio_pin='"${IR_IN_PIN}"'|' /boot/firmware/config.txt
    if ! grep '^dtoverlay=gpio-ir,gpio_pin='"${IR_IN_PIN}"'' /boot/firmware/config.txt; then
      echo "failed to update gpio-ir in"
      exit 1
    fi
  else
    echo "dtoverlay=gpio-ir,gpio_pin=\"${IR_IN_PIN}\"" | sudo tee -a /boot/firmware/config.txt
  fi

  if grep 'gpio-ir-tx,' /boot/firmware/config.txt; then
    sudo sed -i 's|.*gpio-ir-tx,.*|dtoverlay=gpio-ir-tx,gpio_pin='"${IR_OUT_PIN}"'|' /boot/firmware/config.txt
    if ! grep '^dtoverlay=gpio-ir-tx,gpio_pin='"${IR_OUT_PIN}"'' /boot/firmware/config.txt; then
      echo "failed to update gpio-ir out"
      exit 1
    fi
  else
    echo "dtoverlay=gpio-ir-tx,gpio_pin=\"${IR_OUT_PIN}\"" | sudo tee -a /boot/firmware/config.txt
  fi

  echo "IR setup complete"
}

function build {
  echo "build..."
  cargo build
  sudo cp target/debug/raspberry-pi-ir-hat-network-bridge /usr/sbin/raspberry-pi-ir-hat-network-bridge
  echo "build complete"
}

function service_setup {
  echo "setup service..."
  sudo cp scripts/raspberry-pi-ir-hat-network-bridge.service /etc/systemd/system/
  sudo systemctl daemon-reload
  sudo systemctl enable raspberry-pi-ir-hat-network-bridge.service
  sudo systemctl start raspberry-pi-ir-hat-network-bridge.service
  echo "service setup complete"
}

update
dev_setup
rust_setup
adc_setup
ir_setup
build
service_setup
echo ""
echo "Setup complete"
echo ""
echo "To enable readonly run ./scripts/read-only-fs.sh"
echo ""
echo "You may need to reboot to finish setup"
echo ""
