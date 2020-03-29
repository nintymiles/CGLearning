# C++中如何将16进制字符串(hex string)转化为rgb数字矢量表达？

## 1. 关于使用不同进制的数字字面义（literal）表达整数
在计算机中，不管用什么literal格式表达整数，其实都是同一个整数值的不同表达方式。实际上整型（或数值）类型在内存中都是以二进制形式表达的。

因此不管用什么格式表达的整型数字，在计算机中都使用了同一种数值存储方式。

数字输出时则根据不同的表达方式被转化为二进制，十进制和十六进制文字表示。

### 搞清楚C++中literal的作用？
C++中每种基本类型都有对应的字面义(literal)表达,character对应character literal，string对应string literal，integer，float，double对应integer literal和floating-pointer literal。所谓literal就是本身由文字就可以明确的不同类型的值。literal是不同类型值的文字表达方式。

literal通过其值和表达形式决定其类型，数值类型可以通过多种形式的literal来表达。

#### literal例子

```
//比如，一个整数类型可以使用多种不同的literal格式表达来初始化
int a = 152; //十进制
int b = 0xFD23AB; //或0XFD23AB,十六进制
```

## 2. 使用标准库函数实现从数字字符串到整型的转换
C++11标准中包含了库函数stoi、stol、stoll函数，分别执行从不同进位表达的string数字到integer，long，long long整数类型的转化。

如果使用之前的C++版本，则只能使用C库中atoi/atof函数了，但是c版本库函数只支持十进制转换，如果要执行16进制数字转化，则需要自己实现对应版本。

### stox系列函数的使用？
stoi/l/ll系列函数很强大，可以将包含数字的字符串转化为对应格式的数字。以stoi(string* string,size* pos=0,int base=10)为例。其中第一个参数为要转化的字符串，第二参数为起始字符的地址，默认为NULL(0)，第三个参数为字符串树枝的进制格式名称(10代表10进制，16代表16进制）。当只传递string参数时，以十进制方式转化。其中base为16或0时，字符串中的数字可以以"0X"或者“0x”开头。当base为8或0时，字符串中的数字可以以"0"开头。字符串中甚至可以包含"+“或”-"，用以表达有符号整数。

## 3. 将使用16进制表达的色彩RGB字符串转化为整型或者浮点型表达？

所需要的步骤：

1. 将16进位数字字符串转化为整型
2. 对整型变量应用移位操作，获得RGB中每个色彩部件的值

```
    //stoi函数将字符串转化为整型类型
    int num = std::stoi("0xFFFFF",NULL,16);
    //16进制色彩表示中，两个数字一个完整的色彩部件（red，green，blue）
    //2进位存储表达中，8个字节位表达一个16进制整数
    //移位16个字节位，并执行&操作，可以得出red部件的数值
    int red = num>>16&0xFF;
    //移位8个字节位，并执行&操作，可以得出green部件的数值
    int green = num>>8&0xFF;
    //低位8个字节执行&操作，可以得出blue部件的数值
    int blue = num&0xFF;
    
    //如果要得出每个部件数值的浮点表达，执行除法即可
    float redFloat = red/(255.0);
```

## 4. 不使用任何库来转化十六进制表达的数字？

```
//网上扒来的高效率实现
long hextonum(char *in){
    // 用指针循环字符串
    char *pin = in; 
    // 以累加方式获得输出结果
    long out = 0;  

    while(*pin != 0){
        // 针对每一个输入字符，依次移位out到高一级(4位对应一个bit)
        out <<= 4; 
        // 对这个字符进行位运算，并且结果加到out变量的低位上。
        out +=  (*pin < 'A') ? *pin & 0xF : (*pin & 0x7) + 9; 
   
        pin++; 
    }

    return out;
}

// 简单循环实现，一个容易理解，但不是最优的实现
int hexToInt( char* Hex )
{
  int nLen = strlen( Hex );
  int nDigitMult = 1;
  int nResult = 0;
  for( int i = nLen - 1; i >= 0; i-- )
  {
    char ch = Hex[ i ];
    if( '0' <= ch && ch <= '9' )
      nResult += (ch - '0') * nDigitMult;
    else if( 'a' <= ch && ch <= 'f' )
      nResult += (ch - 'a' + 10) * nDigitMult;
    else if( 'A' <= ch && ch <= 'F' )
      nResult += (ch - 'A' + 10) * nDigitMult;
    nDigitMult *= 16;
  }
  return nResult;
}

```

