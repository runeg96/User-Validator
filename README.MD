# User validator

A C++17 command-line tool that reads user records from a JSON file, validates them in two stages (schema + business rules), and exports the valid users to JSON and XML.

## Features

- **Two-stage validation** — JSON schema validation at parse time, then business-rule validation (email format, contact info, platform count)
- **Multiple export formats** — JSON (RapidJSON) and XML (pugixml)
- **Incremental commands** — Run just parsing, parsing + validation, or the full pipeline
- **Structured logging** — Timestamped file logging

## Dependencies

All dependencies are included as git submodules in `external/`

| Library                                            | Version | Purpose                                      |
| -------------------------------------------------- | ------- | -------------------------------------------- |
| [RapidJSON](https://github.com/Tencent/rapidjson)  | master  | JSON parsing, schema validation, JSON export |
| [pugixml](https://github.com/zeux/pugixml)         | v1.15   | XML export                                   |
| [GoogleTest](https://github.com/google/googletest) | v1.14.0 | Unit testing                                 |

## Prerequisites

- A C++17 compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.12 or newer
- Git

### Installing prerequisites

**Ubuntu / Debian**

```bash
sudo apt update
sudo apt install build-essential cmake
```

**Fedora**

```bash
sudo dnf install gcc-c++ cmake make
```

## Building

```bash
git clone <repo-url>
cd json_parser
git submodule update --init --recursive
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

The build copies `users.json` into the build directory automatically.

## Docker

Dockerfiles are provided for Ubuntu and Fedora.

**Ubuntu**

```bash
docker build -f Dockerfile.ubuntu -t json_parser:ubuntu .
docker run --rm json_parser:ubuntu
```

**Fedora**

```bash
docker build -f Dockerfile.fedora -t json_parser:fedora .
docker run --rm json_parser:fedora
```

To run a specific command:

```bash
docker run --rm json_parser:ubuntu ./json_parser parse_users
```

To copy the export files out of the container:

```bash
docker run --rm -v $(pwd)/export:/app/build/export json_parser:ubuntu
```

## Testing

Unit tests are built automatically alongside the main executable. Run them with:

```bash
cd build
ctest --output-on-failure
```

Or run the test executable directly for verbose GTest output:

```bash
./json_parser_tests
```

To build without tests:

```bash
cmake .. -DBUILD_TESTS=OFF
```

## Usage

```
./json_parser [command]
```

### Commands

| Command          | Description                                              |
| ---------------- | -------------------------------------------------------- |
| `parse_users`    | Parse users from `users.json`                            |
| `validate_users` | Parse and validate users                                 |
| `export_users`   | Parse, validate, and export valid users                  |
| `run_pipeline`   | Run the full pipeline (default when no command is given) |

### Options

```
-h, -help, --help, help    Show help message
```

### Examples

```bash
./json_parser                  # Runs the full pipeline
./json_parser parse_users      # Parse only
./json_parser validate_users   # Parse + validate
./json_parser export_users     # Parse + validate + export
./json_parser --help           # Show help
```

## Input Format

The input file (`users.json`) contains an array of user objects with Danish-language keys:

```json
{
  "Navn": "Peter Christensen",
  "Alder": 53,
  "Email": "peter@example.com",
  "Spilleplatform": "PC, PlayStation",
  "Oprettelsesdato": "2009-02-01",
  "Mobilnummer": "94254245",
  "Telefonnummer": null
}
```

## Validation

### Stage 1 — Schema validation (at parse time)

- All required fields must be present (`Navn`, `Alder`, `Email`, `Spilleplatform`, `Oprettelsesdato`)
- Type constraints enforced (strings, integer ≥ 0, nullable phone fields)
- No additional properties allowed
- Records that fail schema validation are rejected and never enter stage 2

### Stage 2 — Business rules

| Rule         | Condition                                                                      |
| ------------ | ------------------------------------------------------------------------------ |
| Email format | Must match a standard email regex (letters, digits, common symbols, valid TLD) |
| Contact info | At least one of mobile or phone number must be provided                        |
| Platforms    | User must have at least 2 gaming platforms                                     |

## Output

Valid users are exported to the `build/export/` directory:

- `users_export.json` — Pretty-printed JSON array
- `users_export.xml` — UTF-8 XML with `<users><user>...</user></users>` structure

Logs are written to `build/logs/parser.log`.
