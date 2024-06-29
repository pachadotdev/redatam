# redatam

*WORK IN PROGRESS*

This program converts Redatam files to CSV files and is a C++14 port of the Redatam converter originally written in C# (https://github.com/discontinuos/redatam-converter).

Compile with:

```bash
make
```

Usage example:

```bash
redatam dev/chile/BaseOrg16/CPV2017-16.dic dev/chile-csv
```

Dependencies:

```
sudo apt-get install libxml2-dev
```