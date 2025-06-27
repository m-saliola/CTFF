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

## License

CTFF is licensed under the GNU General Public License v3.0. See [LICENSE](LICENSE) for details.
