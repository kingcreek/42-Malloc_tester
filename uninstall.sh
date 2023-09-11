#!/bin/bash

if [ "$(uname)" != "Darwin" ]; then
	RC_FILE="$HOME/.bashrc"
	if [[ -f "$HOME/.zshrc" ]]; then
		RC_FILE="$HOME/.zshrc"
	fi
fi

sed -i '/alias malloc_tester=/d' "$RC_FILE"
sed -i '/alias king=/d' "$RC_FILE"

rm -rf "$HOME/.malloc_tester"

echo "Uninstallation completed."
