Implementations in Haskell and C of trivial secret sharing under n
parties so that all n pieces are needed for reconstruction (see
https://en.wikipedia.org/wiki/Secret_sharing#t_=_n).

**To compile** respectively:

    $ ghc xor.hs

or

    $ make xor

**Example calls**, split up the text "Hello World" into five pieces:

    $ echo 'Hello-World' | ./xor out 5

And to reconstruct the original message:

    $ ./xor in out*
    Hello-World
    $
