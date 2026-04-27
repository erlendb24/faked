# Faked

My attempt at recreating ed, the line-based unix editor  

Run by typing ```faked [file]```  

## Commands:

- ```i``` inserts above the current line (insert)  
- ```d``` deletes the current line (delete)  
- ```a``` inserts after the current line (append)  
- ```c``` deletes current line and inserts where it was (change)  
- ```j``` jumps the current line to whichever line you specify (jump)  
- ```P``` print the whole buffer (print_all)  
- ```p``` print the current line (print)  
- ```w``` saves to opened file (write)  
- ```s``` substitutes substring for another ```s/[pattern]/[replace]/``` (substitute)  

## Addresses:

You can use adresses for line numbers in ed.

- numbers
- ```.``` denotes the current line
- ```$``` denotes the last line
- ```,``` denotes a range, used between numbers/symbols, like ```1,3```

### Commands that can be adressed  

- ```a``` can have no argument or single line number as argument (appends after number)
- ```j``` can have no argument or single line number as argument (jumps to line number)
- ```d``` can have no argument, single line number as argument, or a range
- ```c``` can have no argument, single line number as argument, or a range
- ```p``` can have no argument, single line number as argument, or a range

### Examples of adressing:  

```1d``` deletes the first line  

```1,3d``` deletes the three first lines  

```1,$c``` deletes every line between the first and last, and puts you into insert  

```.,$p``` prints every line between the current and last  

```,p``` prints every line  
