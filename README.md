# subnet-calculator

A command-line IPv4 subnet calculator written in C, with no external
dependencies. Give it an address in CIDR notation and it derives the
full subnet layout.

It performs pure arithmetic: no network traffic, no DNS resolution,
works entirely offline.

## Usage

```sh
gcc -Wall -Wextra -o subnetcalc subnet.c
./subnetcalc 192.168.1.10/26
```

Invalid input is reported on `stderr` with a non-zero exit status.

## Design decisions

**Addresses are stored as a single 32-bit integer, not as an array of
octets or a string.** The central operation is `ip & netmask`, which is
one CPU instruction on an integer but a loop on an array. The
representation follows the dominant operation. `uint32_t` guarantees
exactly 32 bits and, being unsigned, keeps the high-order bit a data bit
rather than a sign bit.

**The netmask is built by shifting rather than looked up in a table.**
Starting from `0xFFFFFFFF` and shifting left by `32 - prefix` pushes
zeros in from the right, producing the host portion at no extra cost.

**`/0` is special-cased.** Shifting a 32-bit integer by 32 positions is
undefined behaviour in C: it may appear to work on one machine and
silently produce garbage on another.

**Input is read wide, validated, then narrowed.** Octets are parsed into
`unsigned int` rather than a byte-sized type, so an out-of-range value
such as `999` stays visible instead of wrapping around unnoticed.

**Failure is signalled through the return value, not a sentinel
address.** All 2^32 values of a `uint32_t` are valid IPv4 addresses, so
none is free to mean "error". The parser returns a status code and
writes its results through output parameters.

**No dynamic allocation.** Every value fits on the stack, so memory
leaks are impossible by construction.

**Results go to `stdout`, diagnostics to `stderr`**, so redirecting the
output to a file yields clean data while errors still reach the user.

## Known limitations

- Trailing characters after a valid CIDR string are ignored:
  `192.168.1.1/24xyz` is currently accepted.
- Usable host range and host count are not computed yet.
- IPv4 only. IPv6 is 128 bits and fits no standard integer type, so it
  would need a different representation and different arithmetic.
- The reported class is informational: address classes were superseded
  by CIDR in 1993.

## Requirements

A C99 compiler. Nothing else.

## License

MIT — see `LICENSE`.