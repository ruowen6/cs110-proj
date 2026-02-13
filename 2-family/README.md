# 2-family — All in the Family
This is a C++ console program implementing the COMP.CS.110 assignment "Suku on pahin / All in the family." The program loads a CSV file describing people and parent relationships, stores the information in memory, and provides an interactive command-line interface for family-related queries (children, parents, siblings, cousins, grandparents/grandchildren at N levels, tallest/shortest in lineage, etc.).

Contents of this folder
- `main.cpp` — program entry, CSV parsing and program flow.
- `familytree.hh` / `familytree.cpp` — core data model and query implementations.
- `cli.hh` / `cli.cpp` — command-line interpreter and command-to-function mapping.
- `utils.hh` / `utils.cpp` — simple helpers (string split, numeric checks).
- `data_en.txt`, `data_fi.txt` — example data files (English and Finnish versions).
- `family.pro` — Qt project file (optional).
**High-level behavior**
- Prompts the user for an input file path and attempts to open it.
- Parses the file as semicolon-separated CSV with four fields per non-comment line: `name;height;father;mother`.
- Validates numeric fields (height) and input format; on malformed lines the program prints an error with the line number and exits.
- Builds an in-memory family graph (persons with parent and children pointers) and then launches an interactive REPL to accept commands.

## 1) Background / Purpose
This exercise demonstrates parsing structured text input, representing hierarchical relationships using C++ containers and pointers, and exposing query operations through a clean command-line interface. The implementation emphasizes correct input parsing, clear separation of responsibilities (parsing, data model, CLI), and straightforward use of std::map for ownership-independent lookup.

## 2) How to Use
### Prerequisites
- C++ compiler with C++17 support (e.g., `g++`, `clang++`).
- Optional: Qt (for `family.pro` / `qmake`).

### Build (simple g++)
Open a terminal and run:

```bash
cd 2-family
g++ -std=c++17 -O2 -Wall -Wextra -pedantic main.cpp familytree.cpp cli.cpp utils.cpp -o family
```

This produces the `family` executable.
### Build with Qt (`.pro`)
If you use Qt you can run:

```bash
cd 2-family
qmake family.pro
make
```

### Run
Start the program and provide a path to a CSV data file when prompted:

```bash
./family
Input file: data_en.txt
```

If the data file cannot be opened or contains invalid lines, the program prints an error and exits. On success you will be placed in an interactive prompt (`>`) where you can enter commands.

## 3) Commands and Examples
The CLI recognizes several commands (many have aliases). Commands are case-insensitive. Numeric parameters (for N-level queries) must be numeric.

Common commands and aliases (canonical name / examples):
- `PRINT` / `TREE` / `FAMILYTREE` / `SUKUPUU` — list all stored persons and heights.
- `CHILDREN <person>` / `LAPSET` — print direct children of `<person>`.
- `PARENTS <person>` / `VANHEMMAT` — print parents of `<person>`.
- `SIBLINGS <person>` / `SISARUKSET` — print siblings of `<person>`.
- `COUSINS <person>` / `SERKUT` — print cousins of `<person>`.
- `TALLEST <person>` / `PISIN` — print tallest person in `<person>`'s lineage (including the person themself).
- `SHORTEST <person>` / `LYHYIN` / `LYHIN` — print shortest in lineage.
- `GRANDCHILDREN <person> N` / `LAPSENLAPSET` / `GC` — print grandchildren at distance N (N >= 1).
- `GRANDPARENTS <person> N` / `ISOVANHEMMAT` / `GP` — print grandparents at distance N (N >= 1).
- `QUIT`, `EXIT`, `Q`, `LOPETA` — exit the program.
Examples (interactive):

```
> PRINT
Alice, 170
Bob, 180
> PARENTS Alice
Charlie
> CHILDREN Bob
Diana
> GRANDCHILDREN Alice 2
Alice has 1 great-grandchildren:
Eve
> QUIT
```

If a requested person is not found, the program prints `Error. <name> not found.` For wrong parameter counts the CLI prints `Wrong amount of parameters.` For non-numeric N parameters the CLI prints `Wrong type of parameters.`

## 4) Input file format
- The file is plain text, processed line by line.
- Lines that are empty or start with `#` are ignored (comments).
- Non-comment lines must contain exactly four semicolon-separated fields: `name;height;father;mother`.
	- `name` — string identifier for the person.
	- `height` — integer height (digits only).
	- `father` — father's name (or '-' if unknown).
	- `mother` — mother's name (or '-' if unknown).
- Example line:

```
Alice;170;Bob;Carol
```

The program strictly validates the number of fields and the numeric `height` value; malformed lines cause the program to print `Error in datafile, line <n>` and exit.

## 5) References & Credits
- Based on the COMP.CS.110 Programming 2 assignment "Suku on pahin / All in the family".
- Implementation notes and command mapping are defined in `cli.hh` and `familytree.hh`.