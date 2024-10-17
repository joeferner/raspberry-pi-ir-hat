#!/bin/bash
set -eou pipefail
DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd "${DIR}/.."

echo "setup read only fs..."
sudo raspi-config nonint enable_overlayfs
echo "read only fs setup complete..."

echo ""
echo "To complete setup, reboot"
echo ""
echo "To disable readonly filesystem run the following then reboot"
echo "   sudo raspi-config nonint disable_overlayfs" 
echo ""
