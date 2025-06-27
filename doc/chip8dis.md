# c8dis (CHIP-8 Disassembler)

This is a disassembler for the CHIP-8 and SCHIP, utilizing libc8.

## Usage

```shell
c8dis [-al] [-o outputfile] rom
```

* `-a` toggles printing of addresses.
* `-l` toggles printing of auto-generated labels.
* `-o` writes the output to `outputfile`.
* `-V` prints the version number.

By default, `c8dis` will write to `stdout`.
