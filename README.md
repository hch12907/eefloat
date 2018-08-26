<!-- enable text wrapping if you are reading this in a text editor for best reading experience -->

# eefloat
Emulating PS2's Emotion Engine floats. Converts the single-precision floating points of PS2 to double-precision floating points, and perform double arithmetic.

#### Why?
PS2's floating points are *not* IEEE754 compliant:-

|Exponent  |Mantissa|In IEEE754      |In the PS2      |
|:--------:|:------:|:--------------:|:--------------:|
|  +128    |   >0   |      NaN       |Normalized Value|
|  +128    |    0   |    Infinity    |Normalized Value|
|-127<E<128|   Any  |Normalized Value|Normalized Value|
|  -127    |   >0   |Subnormal Value |       0        |
|  -127    |    0   |        0       |       0        |

Therefore, some large floats in the PS2 are not emulatable in x86 due to Infinity/NaN propagation.
By doing a single<->double conversion, the +128 exponent is freed up (1024 is used for Inf/NaNs in doubles).

#### How?
1. Manually cast the float into a double (usual way of casting causes NaN propagation)
2. Perform the arithmetic
3. Manually cast the double back into a float (if EXP>255, return 0xFFFFFFFF; if EXP<=0, return 0x0)
