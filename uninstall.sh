#!/bin/bash

sed -i '/alias malloc_tester=/d' "$HOME/.zshrc"
sed -i '/alias king=/d' "$HOME/.zshrc"

rm -rf "$HOME/MALLOC_TESTER"

echo "Uninstallation completed."