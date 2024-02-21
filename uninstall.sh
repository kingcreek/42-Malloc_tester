#!/bin/bash

RC_FILE="$HOME/.zshrc"

if [ "$SO" == "linux" ]; then
	RC_FILE="$HOME/.bashrc"
	# if [[ -f "$HOME/.zshrc" ]]; then
	# 	RC_FILE="$HOME/.zshrc"
	# fi
fi

sed -i'.bak' '/^alias malloc_tester=/d' $RC_FILE
sed -i'.bak' '/^alias king=/d' $RC_FILE
sed -i'.bak' '/^alias amparette=/d' $RC_FILE


rm -rf "$HOME/.malloc_tester"
echo "Uninstallation completed."

exec "$SHELL"