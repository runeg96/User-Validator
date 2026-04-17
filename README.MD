# User Validator

A C++17 command-line tool that reads user records from a JSON file, validates them in two stages (schema + business rules), and exports valid users to JSON and XML.

## Getting Started

### Prerequisites

- C++17 compiler
- CMake 3.12+
- Git

**Ubuntu / Debian:** `sudo apt install build-essential cmake`
**Fedora:** `sudo dnf install gcc-c++ cmake make`

### Building

```bash
git clone https://github.com/runeg96/User-Validator.git
cd json_parser
git submodule update --init --recursive
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Running

```bash
cd build
./user_validator                  # Full pipeline
./user_validator parse_users      # Parse only
./user_validator --help           # Show help
```

## Usage

```bash
./user_validator [command]
```

| Command          | Description                                              |
| ---------------- | -------------------------------------------------------- |
| `parse_users`    | Parse users from `users.json`                            |
| `validate_users` | Parse and validate users                                 |
| `export_users`   | Parse, validate, and export valid users                  |
| `run_pipeline`   | Run the full pipeline (default when no command is given) |

## Dependencies

All included as git submodules in `external/` — no system packages needed.

| Library    | Purpose                                      |
| ---------- | -------------------------------------------- |
| RapidJSON  | JSON parsing, schema validation, JSON export |
| pugixml    | XML export                                   |
| GoogleTest | Unit testing                                 |

## Testing

```bash
cd build
ctest --output-on-failure
```

To build without tests: `cmake .. -DBUILD_TESTS=OFF`

## Docker

```bash
docker build -f Dockerfile.ubuntu -t user_validator:ubuntu .
docker run --rm user_validator:ubuntu
```

A Fedora Dockerfile is also provided (`Dockerfile.fedora`).

## Validation

### Stage 1 — Schema (at parse time)

- Required fields present (`Navn`, `Alder`, `Email`, `Spilleplatform`, `Oprettelsesdato`)
- Correct types enforced (strings, integer ≥ 0, nullable phone fields)
- No additional properties allowed

### Stage 2 — Business rules

| Rule         | Condition                                       |
| ------------ | ----------------------------------------------- |
| Email        | Must be a valid email format                    |
| Contact info | At least one of mobile or phone number provided |
| Platforms    | At least 2 gaming platforms                     |

Records failing stage 1 are rejected before reaching stage 2. All errors are logged to `build/logs/parser.log`.

## Output

Exported to `build/export/`:

- `users_export.json` — JSON
- `users_export.xml` — XML
