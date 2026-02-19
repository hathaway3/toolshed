# casm (6809/6309 Assembler)

`casm` is a Motorola MC6809 and Hitachi HD6309 cross-assembler. It is designed for high performance and compatibility with various legacy assemblers.

## Usage
`casm [files] [-options]`

### General Options
| Option | Description |
| --- | --- |
| `-help`, `-?` | Displays the help list |
| `-6809` | Allow only 6809 opcodes |
| `-D <sym>[=<val>]` | Define symbol |
| `-I <dir>` | Add include directory |
| `-C` | Use case-sensitive labels |
| `-silent` | Run in silent mode |
| `--no-warn` | Disable warnings |

### Output Options
| Option | Description |
| --- | --- |
| `-O <dir>` | Set output directory |
| `-o <file>` | Set output name |
| `-bin` | Generate CoCo RS-DOS binary file |
| `-s19` | Generate S-record file |
| `-rom[=size]` | Generate a padded ROM file (optional size in KB) |
| `-raw` | Generate raw binary file |
| `-mod` | Generate shared object module |
| `-os9` | Generate Microware OS-9 module |
| `-rof` | Generate Microware Relocatable Object File |
| `-obj` | Generate CASM object file |
| `-noout` | Assemble only, do not generate output file |

### Compatibility Modes
`casm` provides several modes to assemble source code written for other assemblers:
| Option | Description |
| --- | --- |
| `--mode-rma` | Assemble in RMA compatibility mode |
| `--mode-macro80c` | Assemble in Macro-80c compatibility mode |
| `--mode-edtasm` | Assemble in EDTASM+ compatibility mode |
| `--mode-edtasm6309` | Assemble in EDTASM6309 compatibility mode |
| `--mode-ccasm` | Assemble in CCASM compatibility mode |

### Advanced Options
| Option | Description |
| --- | --- |
| `--warn-portable` | Warn of source code portability issues |
| `--force-zero-offset` | Enable explicit 0 index offset |
| `--force-pc-relative` | Use relative offsets for both PC and PCR |
| `--disable-macros` | Disable use of macros |
| `--disable-locals` | Disable use of local labels |
| `--strict-locals` | Enable use of strict local label rules |
| `--ignore-case` | Ignore case on symbols |
| `--enable-precedence` | Enable operator precedence in expressions |

### Listing Options
| Option | Description |
| --- | --- |
| `-list` | Generate formatted source file listing to stdout |
| `-nolist` | Source output disabled |
| `-cycle` | Cycle count enabled |
| `-slist` | Generate formatted list of labels |
| `-xref` | Cross reference table |
| `-expand` | Macro definitions will be expanded in listing |
| `-opt` | Display number of lines that could be optimized |
| `-alert` | Display source lines that could be displayed |
| `-noln` | Do not print line numbers in listing |
| `-noopdata` | Do not print opcode data in listing |
| `-cm` | Comment out macros in listing |
