# CTFF - Compressed Text For Frogs

CTFF is a simple dictionary-based compressor for text files. It replaces known words with compact indices to reduce file size.

## Features

- Compress `.txt` files into `.ctff`
- Decompress `.ctff` files back to text
- Handles unknown words and common suffixes
- Dictionary-based for efficiency and flexibility

## Installation

Compile with `g++`:

``` 
g++ main.cpp -o ctff
``` 

Requires C++17 or higher.
**Make sure to have the english dictionary in the dict folder next to the CTFF program.**

## Usage

To compress:

``` 
ctff example.txt english
``` 

To decompress:

``` 
ctff example.ctff english
``` 

If no dictionary is given, it defaults to `english.txt` (Google’s 10K word list).

## Explanation

CTFF compresses files by scanning each word and matching it against a dictionary.  
Known words are replaced with their index, encoded in base 92 for compactness.  
Unknown words are preserved with a fallback marker.  
Capital letters are stored separately using a compact bitmap format.  
Decompression reverses this process to reconstruct the original text.

## Dictionaries

Dictionaries are `.txt` files with one word per line.

Example:
``` 
the
quick
brown
fox
jumps
over
the
lazy
dog
...
``` 

To create your own, just save it as a `.txt` file **in the dict folder next to the program** and pass the name (without extension) as the second argument.

``` 
ctff example.txt mydict
```

## Compression

CTFF reduces file size by roughly **25%**, depending on dictionary coverage and capitalization. Capital text adds a lot of data, so minimizing capitals will result in better compression.

## Add CTFF to Right-Click Menu

You can integrate CTFF into your operating system's context menu to compress files more easily.

### Windows

1. Create a `.reg` file (e.g., `ctff-context.reg`) with the following content:

    ```reg
    Windows Registry Editor Version 5.00

    [HKEY_CLASSES_ROOT\*\shell\CTFF Compress]
    @="Compress with CTFF"
    "Icon"="C:\\Path\\To\\ctff.exe"

    [HKEY_CLASSES_ROOT\*\shell\CTFF Compress\command]
    @="\"C:\\Path\\To\\ctff.exe\" \"%1\""
    ```

2. Replace `C:\\Path\\To\\ctff.exe` with the actual full path to your CTFF executable.
3. Double-click the `.reg` file to add it to the registry.
4. You’ll now see "Compress with CTFF" when right-clicking `.txt` files.

Note: You can also create a version that works for folders if needed.

---

### Linux (Nautilus, Nemo, Thunar)

#### For GNOME Files (Nautilus)

1. Create a script file:

    ```bash
    ~/.local/share/nautilus/scripts/CTFF Compress
    ```

2. Paste the following into the file:

    ```bash
    #!/bin/bash
    for file in "$@"; do
        ctff "$file"
    done
    ```

3. Make it executable:

    ```bash
    chmod +x ~/.local/share/nautilus/scripts/CTFF\ Compress
    ```

4. Right-click any `.txt` file → Scripts → CTFF Compress

#### For Other File Managers

Most Linux file managers support custom actions:

- **Nemo**: `~/.local/share/nemo/actions/`
- **Thunar**: Use *Edit → Configure custom actions*

Make sure `ctff` is in your PATH, or use the full path to the binary in the script.

## Notes

- Currently, CTFF does not support MacOS.
- If CTFF doesn't run unless you prefix it with ./ or provide the full path, you may need to [add it to your system PATH](https://zwbetz.com/how-to-add-a-binary-to-your-path-on-macos-linux-windows/). This lets you run CTFF from anywhere.

## License

CTFF is licensed under the GNU General Public License v3.0. See [LICENSE](LICENSE) for details.
