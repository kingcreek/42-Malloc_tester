#!/bin/bash

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    SO="linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    SO="macos"
else
    echo "Unsupported operating system."
    exit 1
fi

FOLDER=".malloc_tester"

mkdir -p "$HOME/$FOLDER"

if [ "$SO" == "linux" ]; then
    curl -L -o "$HOME/$FOLDER/malloc_tester.so" "https://github.com/kingcreek/42-Malloc_tester/raw/main/malloc_tester.so"
elif [ "$SO" == "macos" ]; then
    curl -L -o "$HOME/$FOLDER/malloc_tester.dylib" "https://github.com/kingcreek/42-Malloc_tester/raw/main/malloc_tester.dylib"
fi
curl -L -o "$HOME/$FOLDER/malloc_tester.sh" "https://github.com/kingcreek/42-Malloc_tester/raw/main/malloc_tester.sh"

chmod +x "$HOME/$FOLDER/malloc_tester.sh"

echo "alias malloc_tester=\"$HOME/$FOLDER/malloc_tester.sh\"" >> "$HOME/.zshrc"  
echo "alias king=\"$HOME/$FOLDER/malloc_tester.sh\"" >> "$HOME/.zshrc"

echo "alias malloc_tester=\"$HOME/$FOLDER/malloc_tester.sh\"" >> "$HOME/.bashrc"  
echo "alias king=\"$HOME/$FOLDER/malloc_tester.sh\"" >> "$HOME/.bashrc"

echo "Configuration completed. You can run 'malloc_tester' to run the program."
