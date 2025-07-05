# c8as (CHIP-8 Assembler)

This is an assembler for the CHIP-8 and SCHIP, utilizing libc8.

## Usage

```shell
c8as [-vV] [-o outputfile] src
```

* `-o` sets an output file (default is `a.c8`).
* `-v` prints diagnostic messages and the resulting hex-encoded bytecode to standard output.
* `-V` prints the version number.

## Example Code

```
JP mylabel

mysprite:
DW 0x1234

mylabel:
ADD V0, 1
ADD V1, 0xA
LD I, mysprite
DRW V0, V1, 1
```

## Notes

* Hex integers must be formatted with `0x`, `x`, or `$` prefixes.
* Binary integers must be formatted with a `0b` prefix.
* Data bytes may be defined using the `.DB` symbol.
* 16-bit data words may be defined using the `.DW` symbol.
* Commas are optional for instruction parameters.
* This assembler is not case sensitive.
