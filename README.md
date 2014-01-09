Asymmetric-Encryption-Library
======================

An open source implementation of Asymmetric Encryption (RSA and ECC).

*Note: This is a work in progress. Please refer to checklist at bottom for progress.*

Getting Started
===========

You'll need:

 * Visual Studio 2012+ x64 on a x64 Windows machine
 * The code along with all solution/project files

Download the code and launch the solution file **RSA Erdelsky.sln**. Go to Build -> Configuration Manager. Under Active solution configuration, select Release. Under Active solution platform, select x64. Ctrl+F5 to run without debugging and follow the text UI for testing encryption/decryption cycle.

How to actually make use of this code
============================

You probably don't want to just test whether or not the code is working so here's a bit more detailed explanation of what does what:

Multiple Precision Unsigned Integers
----------------------------------------------------

This project utilizes a custom-written mpuint library to handle large numbers. The large integers are essentially based on arrays of "chunks" of a specified size. The size of the individual chunks is specified as a preprocessor directive in **mpuint.h**. It has 3 supported values:

``` cpp
    #define BITS_IN_CHUNK 32 //this will use 32-bit integers
    #define BITS_IN_CHUNK 16 //this will use 16-bit integers
    #define BITS_IN_CHUNK 8 //this will use 8-bit integers
```

*Note: the maximum chunk size on 64-bit machine is 32-bit so that the full result of 32-bit multiplication can be stored. Also, for performance reasons, although 64-bit integers are available on 32-bit machines (by using the same idea as this mpuint), choose 16-bit integers for 32-bit OS.*

The mpuint class uses assembly implementations of some functions as long as the USE_ASSEMBLY_IMPLEMENTATIONS directive is enabled. If you aren't using Visual Studio 2012 and cannot enable 64 bit assembly parsing, remove the directive. If you are using Visual Studio 2012 and are still having problems, refer to Potential Issues below. Only 32-bit chunks have assembly implementations, which take on average ~~5-10%~~ up to 40% less time.

*Note: When using assembly implementations, mpuints can only contain an even number of chunks (part of the performance benefit is treating each pair of 32-bit chunks as a single 64-bit).*

You can create an mpuint through one of four ways (three constructors):

    unsigned len = 16;
    AsymmEL::mpuint a(len); \\will create an mpuint with 16 chunks (512-bit if using 32-bit chunks)
    AsymmEL::mpuint b(a); \\will create an exact copy of an mpuint (both a and b will contain the same garbage value)
    AsymmEL::mpuint c("e8e960c75ce543b17638885d40ad881e", 16); \\c will be a 512-bit integer equal to 0xe8e960c75ce543b17638885d40ad881e
    AsymmEL::mpuint d("e8e960c75ce543b17638885d40ad881e") \\d will be calculated to need 128 bits(4 chunks) to store the same value if the second parameter is ommited.

After you've created the object, you can use the various arithmetic implemented in the code. Binary arithmetic (a+b, a%b) is much slower due to creating temporary copies of objects. Use compound assignment operators (a += b) as much as possible to take advantage of the optimized overloads.

Use the shift(n) method to shift an mpuint n bits up.

Also available are comparison operators with both other mpuint and individual chunks (e.g. regular int).

Finally, the (<<) stream operator is overloaded to output a human-readable representation of an mpuint using any standard output stream.

*Mention: There is a finite_mpuint class which inherits mpuint and is currently used in ECC. It's essentially an mpuint that mods by a base at every operation.*

Rivest-Shamir-Adleman Algorithm
-------------------------------------------------

This code contains a fully-functional RSA implementation using the mpuint class above for bigger key sizes. If you're curious, Wikipedia has a [nice explanation](http://en.wikipedia.org/wiki/RSA_%28algorithm%29) of what RSA is . You can use the implementation in one of two ways:

**Using the Message class**

The easiest way to use RSA is to utilize the pre-built Message class, like so:

    unsigned short keySize = 32; //for a total of 1024-bit
    string originalMessage = "test";
    AsymmEL::Message theMessage = AsymmEL::Message(keySize,originalMessage.c_str(),(unsigned)originalMessage.size());

    //at this point the message is embedded into the Message object in memory as mpuints
    //Note that the Message constructor accepts a void* and a size so any block of contiguous memory can be embedded.
    //You can also use embedMessage with a void* and a size to embed a different message at any point

    //generate RSA keys
    theMessage.generateKeys();

    //Note you could also specify pre-generated public and/or private keys by using:
    //theMessage.setPublicKey(e,n);
    //theMessage.setPrivateKey(d,n);

    //encrypt and decrypt
    theMessage.encryptMessage();
    theMessage.decryptMessage();

    //extract the message again
    char buff[501];
    int bytesRead = theMessage.extractMessage(buff,500);
    buff[bytesRead] = 0;
    string decryptedMessage = string(buff);

**Encrypting and decrypting yourself**

Well, RSA is just raising a number to a power so, if you wanted to, you could use the methods in mpuint to do everything. To make your job a bit easier, there are some helper methods along the way as well. Refer to the code below for step-by-step instructions

    /*
    First, you need an mpuint to store your data.
    Usually compiling your mpuint out of individual bytes
    of your data seems to work. One thing you should
    keep in mind is to keep the most significant bit a 0
    (thus, the most significant byte should be no bigger than 0x7F)
    Also, padding is optional but recommended
    ( i.e. leave as little leading 0x00 bytes as you can)
    */
    unsigned int keySize = 32;
    AsymmEL::mpuint data(keySize);
    
    //populate your data mpuint(s)

    //as before, you still need to generate keys
    //but this time you have to manage them yourself.
    AsymmEL::mpuint d = AsymmEL::mpuint(keySize),e = AsymmEL::mpuint(keySize),n = AsymmEL::mpuint(keySize);
    GenerateKeys(d,e,n);

    //Finally, just raise your message to e and d to encrypt and decrypt
    AsymmEL::mpuint encrypted(keySize);
    AsymmEL::mpuint decrypted(keySize);
    AsymmEL::mpuint::Power(data,e,n,encrypted);
    AsymmEL::mpuint::Power(encrypted,d,n,decrypted);

Benefit of this method is that you can now actually transfer the encrypted data over a network or any insecure channel (an anticipated feature on Message as well), which is pretty much the only thing Asymmetric Encryption was made for.

Elliptic Curve Algorithm
----------------------------------

Coming soon? This really needs a lot of work so use at your own risk. The ECC code itself is worth a look out of academic curiosity. For reference, below is the part of the code that handles ECC, verbatim.

    using namespace AsymmEL;

    unsigned keySize = EllipticCurve::sizes[choice]/BITS_IN_CHUNK;

    mpuint prime(EllipticCurve::bases[choice],keySize);
    finite_mpuint a(EllipticCurve::coefficients[choice][0],prime,2*keySize),
        b(EllipticCurve::coefficients[choice][1],prime,2*keySize),
        c(EllipticCurve::coefficients[choice][2],prime,2*keySize),
        x(EllipticCurve::points[choice][0],prime,2*keySize),
        y(EllipticCurve::points[choice][1],prime,2*keySize);
    EllipticCurve ec(a,b,c);
    ECPoint P(ec,x,y);

    finite_mpuint d(2*keySize,prime);
    Random(d);
    d %= prime;

    ECPoint Q(ec,x,y);
    Q *= d;

    ECMessage theECMessage(prime,originalMessage.c_str(),(unsigned)originalMessage.size());

    theECMessage.encryptMessage(P,Q);
    theECMessage.decryptMessage(d);

    char buff[501];

    int bytesRead = theECMessage.extractMessage(buff,500);

    buff[bytesRead] = 0;

    decryptedMessage = string(buff);

Not horrible but could definitely use some usability improvements.

Potential Issues
============

**undefined symbol RAX , RBX**

RAX, RBX, etc. are 64-bit registers used by the Assembly code. Make sure you compile for x64 Platform. If you're using software different from Visual Studio 2012, you will need to set up 64-bit assembler or comment out #define USE_ASSEMBLY_IMPLEMENTATIONS in mpuint.h to remove assembly parsing altogether.



Checklist
=======

Things that are currenly worked on or will be visited soon:

 * Document setup, usage, code
 * Implement hardware solution for true random generator
 * Network functionality for Message class
 * Significantly improve ECC speed
 * Compile actual lib/dll for use in projects
 * Modify code for:
   * ~~32-bit Windows~~
   * Visual Studio 2010
   * Unix flavors
 * Validate key size input in demo?
