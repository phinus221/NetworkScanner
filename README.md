# Lightweight Network Port Scanner

This project is a simple TCP port scanner written in C++ to help me learn and practice networking concepts such as sockets, non-blocking connections, and basic port scanning techniques.

---

## Features

- Resolves hostnames to IP addresses
- Checks if the host is up using `ping`
- Scans all TCP ports (1-65535) to find open ports
- Identifies common services on well-known ports
- Sends a basic HTTP request on ports 80 and 8080 to get server response

---

## Requirements

- Linux operating system (tested on Linux)
- C++ compiler (tested with g++)
- No additional dependencies beyond standard Linux networking libraries

---

## Compilation

Use the included `Makefile` to build the scanner:

```bash
make

