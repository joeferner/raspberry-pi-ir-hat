# Setup Raspberry Pi

1. Install rpi-imager locally
1. Edit Settings
   - hostname: ir
   - set username/password
   - (optional) configure wireless lan
   - set locale settings
   - enable ssh
1. Start the Raspberry Pi, open a command prompt and `ping ir.local`
1. Open a terminal

        ssh <username>@ir.local
        sudo apt update
        sudo apt -y install git
        # copy id_rsa from host computer
        chmod 600 ~/.ssh/id_rsa
        ssh-keygen -p -f ~/.ssh/id_rsa # remove password
        git clone git@github.com:joeferner/raspberry-pi-ir-hat.git
        ./raspberry-pi-ir-hat/scripts/raspberry-pi-setup.sh

# Raspberry Pi development

1. Install "Remote Development" extension pack for VSCode.
1. Connect VSCode via ssh (Ctrl+Shift+P -> Remote-SSH: Connect to Host...) `<username>@ir.local`
1. Stop service `sudo systemctl stop raspberry-pi-ir-hat-network-bridge`
1. Run `RUST_BACKTRACE=1 LOG_LEVEL=debug cargo run`
