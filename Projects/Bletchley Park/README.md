# Bletchley-Park

Linux-use only

Use makefile to build code
```
make
```

Program takes one parameter which is the number of decryption
threads to run, for example for six threads:
```
./decrypt 6
```
Exit execution once "Time to shutdown message" is output
"ctrl+c"

To verify correct decryption type, compare with decrypted
plaintext, produces no output after valid decryption
```
diff results/ plaintext/ -x .gitignore
```
