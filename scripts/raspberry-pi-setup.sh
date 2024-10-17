#!/bin/bash
set -eou pipefail
DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd "${DIR}/.."

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
build
service_setup
echo ""
echo "Setup complete"
echo ""
echo "To enable readonly run ./scripts/read-only-fs.sh"
echo ""
echo "You may need to reboot to finish setup"
echo ""
