# redatamconverter

*WORK IN PROGRESS*

This program converts Redatam files to CSV files and is a C++ 11 translation of:
1. The Redatam converter originally written in C# (https://github.com/discontinuos/redatam-converter).
2. The redatam4r package (https://github.com/FranciscoDA/redatam4r/)

Works on Linux/Mac/Windows but needs extra work.

## Usage

```
make && ./redatamconverter inp/Censo2017_Microdatos_Redatam/CPV2017.dicx out
```

You can check the out folder in this repository and check that the files are not perfect.

Problems so far:

1. Special characters are dropped
2. The CSV files contain strange symbols in the REDCODEN column
