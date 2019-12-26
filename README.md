# ELF Hacking

Some notes and snippets about playing with ELF files.

## Insights

`p_offset` has to be page-aligned. Therefore even the ELF header and the Program Header Table is loaded into the first segment in order to avoid a large padding inside the file which would be necessary when the code needs to start at a page-aligned address.
