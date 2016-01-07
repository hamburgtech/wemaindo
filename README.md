# wemaindo
Web Matrix Interface with Arduino 

This git contains an Arduino sketch that is compatible to most Arduino boards and the Arduino Ethernet Shield. Also included
is an index.htm file that should be stored on a mirco sd card and then plugged into the Arduino Ethernet Shield.

To use the Arduino Sketch you will have to make some changes:

1. Find out what your routers IP is, as a Windows user you do RUN -> enter "cmd" -> then enter in the console: "ipconfig /all"
   The standard gateway is your routers IP for example 192.168.150.1
2. Now you can change the IP of the Arduino in the Sketch to for example 192.168.150.140 (it must be your routers IP: 192.168.Router.xxx)
3. I am using the FastLED libary and set it up for a 11 x 11 WS2812B LED Matrix, if you use other LEDs or more or less LEDs you have to change this part.
4. In the index.htm there is also the possibility to change the size of the LED Matrix. But it's not that easy to change the LED order that I use. But it's possible ;-)
                  
                My LED Matrix WS2812B Stripe (order):
                11  10  9   8   7   6   5   4   3   2   1 <- signal from Arduino / + 5 volt / ground
                12  13  14  15  16  17  18  19  20  21  22
                33  32  31  30  29  28  27  26  25  24  23
                34  35  36  37  38  39  40  41  42  43  44
                55  54  53  52  51  50  49  48  47  46  45
                56  57  58  59  60  61  62  63  64  65  66
                77  76  75  74  73  72  71  70  69  68  67
                78  79  80  81  82  83  84  85  86  87  88
                99  98  97  96  95  94  93  92  91  90  89
                100 101 102 103 104 105 106 107 108 109 110
+5 volt/GND ->  121 120 119 118 117 116 115 114 113 112 111
