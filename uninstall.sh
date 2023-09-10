#!/bin/bash

sed -i '/alias malloc_tester=/d' "$HOME/.bashrc"
sed -i '/alias king=/d' "$HOME/.zshrc"

sed -i '/alias malloc_tester=/d' "$HOME/.bashrc"
sed -i '/alias king=/d' "$HOME/.zshrc"

rm -rf "$HOME/.malloc_tester"

echo "Uninstallation completed."