There is a race condition when a user passes Perl code with the command line 'e' option. Here's a plan that Perl 1 implements:
1. Create a temporary file prefixed with "perl-e" by calling a 'mktemp()' function.
2. Write Perl 1 code passed as a program argument to the newly created file.
3. Close the temporary file flushing the buffers.
4. Proceed as if the temporary file would be passed to the Perl 1 interpreter.
Suppose you want to print a 'hello, world' text with Perl 1.
```
$ ./perl -e 'print "hello, world\n";'
```
It would create a temporary file (e.g., /tmp/perl-emMuFDJ). That file is certain to contain the Perl code followed by a new line character. Then, the execution proceeds as if you execute:
```
$ ./perl /tmp/perl-emMuFDJ
```
The temporary file is deleted upon program termination. It should properly print the "hello, world" text. Let's take a closer look at the implementation in the perly.c file. I've inserted my code comments to make it easier to understand.
```
case 'e':
    if (!e_fp) {
		/* This pattern is '/tmp/perl-eXXXXXX'. */
        e_tmpname = strcpy(safemalloc(sizeof(TMPPATH)),TMPPATH);
		/* Create a temporary file prefixed with "perl-". */
        mktemp(e_tmpname);
		/* It would now open the temporary file for writing. */
        e_fp = fopen(e_tmpname,"w");
    }
    if (argv[1])
        fputs(argv[1],e_fp); /* Write a Perl code passed in a command-line argument. */
    putc('\n', e_fp); /* Add a new line to the Perl code. */
    argc--,argv++;
    break;
```
The two first points of the plan implemented are done by now. The Perl code is likely to reside in a buffer, and the temporary file is almost certain of size 0. The rest is done at the end of the switch().
```
switch_end:
  if (e_fp) {
      /* Close the temporary file. */
      fclose(e_fp);
	  /* Set a new program path. */
      argc++,argv--;
      argv[0] = e_tmpname;
  }
```
Perl 1 interpreter will now proceed as if the temporary file was passed in a command-line argument. The moment just after it calls 'fclose()' is when the race conditions occur. It can be exploited by writing a possibly malicious code in the already created temporary file. In our example, it's /tmp/perl-emMuFDJ. to be vulnerable, an attacker must have write access to that temporary file. There are several cases when it can be done:
1. he uses the same user, as the 'perl' command executor,
2. he's 'root' and executes 'chmod -t /tmp' first on modern operating systems,
3. the /tmp directory otherwise allows this file operation.
The exploit allows the attacker to run a possibly malicious Perl code. The [Perl Harbor](https://gist.github.com/Stagyrite/2175863894712ae71c129f7012d34da3 "Security issues · eltikia/Perl-1.0 · Discussion #59") exploit swaps user programs with one printing 'owned by stagyrite'. Here are two real-world examples:
```
$ ./perl-harbor &
[1] 5406
$ ./perl -e 'print "hello, world\n";'
owned by stagyrite
```
```
$ ./perl-harbor &
[1] 5426
$ ./perl -e 'print "hello, world\n";'
$ ./perl -e 'print "hello, world\n";'
hello, world
[1]+  Fertig                  ./perl-harbor
$ ./perl-harbor &
[1] 5432
$ ./perl -e 'print "hello, world\n";'
$ ./perl -e 'print "hello, world\n";'
hello, world
[1]+  Fertig                  ./perl-harbor
$ ./perl-harbor &
[1] 5435
$ ./perl -e 'print "hello, world\n";'
owned by stagyrite
```
Here's a plan implemented:
1. Change the working directory to /tmp where Perl 1 temporary files reside.
2. Loop until any function confirms that the system has been just exploited.
3. Scan the /tmp directory for files starting with 'perl-e'.
4. Wait until the Perl program is written by the 'perl' process. It's done by repetitively getting a file status. A single call to the 'stat()' function should be enough.
5. Open the Perl program for writing, truncate it and write another Perl code. The other Perl code prints 'owned by stagyrite', although it could be anything that compiles.
6. Perl Harbor now exists leaving.

Exploitation is successful when Perl 1 doesn't manage to execute its code before it gets swapped with the infected one. It can be unsuccessful if Perl Harbor swaps the file too early or too late. Thus, there are 4 possible endings:
1. The exploit doesn't detect any temporary file.
2. The file will be truncated, and therefore won't compile.
3. The file will be original, and therefore exploitation is unsuccessful.
4. This file will be swapped, and therefore exploitation is successful.

You can get the complete exploit in C programming language from [GitHub](https://gist.github.com/Stagyrite/2175863894712ae71c129f7012d34da3 "Perl Harbor: exploit against Perl Kit 1.0").
