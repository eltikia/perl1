The [Perl 1](https://github.com/stagyrite/Perl-1.0 "Stagyrite/Perl-1.0: 🐪 Perl Kit, Version 1.0") interpreter is vulnerable to an attack that has to do with temporary files. Race conditions exist when a program is passed with 'e' command line option. Here's an example traditional Hello World program.
```
$ ./perl -e 'print "hello, world\n";'
```
This creates a file like /tmp/perl-eO3gdPn that can be overwritten by an external process with another program in Perl language. It can for example print "owned by stagyrite" instead of "hello, world". There is a proof-of-example exploit named [Perl Harbor](https://gist.github.com/Stagyrite/2175863894712ae71c129f7012d34da3 "perl-harbor.c"). Let's observe how to use it against a Perl 1 interpreter in a sample session.
```
$ ./perl -e 'print "hello, world\n";'
hello, world
$ ./perl-harbor.c &
[1] 2146125
$ ./perl -e 'print "hello, world\n";'
owned by stagyrite
[1]+  Done                    ./perl-harbor.c
```
As one can see, Perl 1 isn't secure when it comes to handling temporary files with its programs. Our exploit managed to overwrite the user input. It might require several retries before the exploit succeeds in infecting the temporary file. It's because Perl 1 might overwrite the exploit's file too in one of the lines in perly.c.
```
  switch_end:
    if (e_fp) {
        fclose(e_fp);
        argc++,argv--;
        argv[0] = e_tmpname;
    }
```
The exploit has to write the file after the 'fclose()' function is called. The code below comes from the perly.c file, and shows how Perl 1 starts writing a temporary file.
```
        case 'e':
            if (!e_fp) {
                e_tmpname = strcpy(safemalloc(sizeof(TMPPATH)),TMPPATH);
                mktemp(e_tmpname);
                e_fp = fopen(e_tmpname,"w");
            }
            if (argv[1])
                fputs(argv[1],e_fp);
            putc('\n', e_fp);
            argc--,argv++;
            break;
```
One might want to replace a call to function 'mktemp()' with 'mkstemp()' but that wouldn't fix this particular security issue at all.

