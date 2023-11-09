# 在 C++ 中获取以秒(或毫秒)为单位的系统时间

大多数计算机系统使用 [Unix 时间](https://unixtime.org/)，它代表从 1970 年 1 月 1 日 00:00:00 UTC时间 开始经过的秒数，（又称为 Unix纪元，POSIX时间，Unix时间戳等）。下面总结一下几种C++获取unix时间的方法及易混淆的地方。

## 使用C风格的函数std::time()获取时间

### [C++ 中使用 `time()` 函数获取时间（毫秒）](https://www.delftstack.com/zh/howto/cpp/how-to-get-time-in-milliseconds-cpp/#c-中使用-time-函数获取时间毫秒)

在 C++ 中，可以调用`<ctime>`中的 `time` 函数。`time` 可以接受一个类型为 `time_t*`的可选out参数，若传递则返回的时间将存储在这个指针参数中。如果不传递可选参数（传递一个空指针nullptr），则可以从函数返回值中获取当前系统时间。注意，time函数只能返回以秒为单位的系统时间，无法精确获得毫秒值。

```c
#include <ctime>
#include <iostream>

int main() {
  using std::cout,std::endl;
  
  time_t now = time(nullptr);
  time_t mnow = now * 1000;

  cout << "seconds since epoch: " << now << endl;
  cout << "milliseconds since epoch: " << mnow << endl << endl;

  return EXIT_SUCCESS;
}
```

输出：

```text
seconds since epoch: 1699499776
milliseconds since epoch: 1699499776000
```

## 使用现代C++的chrono库

### [使用 `std::chrono::system_clock::now()` 方法在 C++ 中获取以毫秒为单位的时间](https://www.delftstack.com/zh/howto/cpp/how-to-get-time-in-milliseconds-cpp/#使用-stdchronosystem_clocknow-方法在-c-中获取以毫秒为单位的时间)

`std::chrono::system_clock` 类是 C++ 中获取全系统实时挂钟的接口。

首先，调用 `now()` 方法来返回当前的时间点。接下来调用的方法是 `time_since_epoch` 来当前时间点和unix时间纪元之间的时间差值，返回一个 `std::chrono::duration` 对象。这个对象可以调用 `count` 方法来返回整型时间量，要以毫秒为单位来表示，需要使用 `duration_cast<milliseconds>` 进行转换。

```cpp
#include <chrono>
#include <iostream>

int main() {
	using std::cout,std::endl;
  using std::chrono::duration_cast,std::chrono::system_clock;
  using std::chrono::milliseconds,std::chrono::seconds;

  auto millisec_since_epoch =
      duration_cast<milliseconds>(system_clock::now().time_since_epoch())
          .count();
  auto sec_since_epoch =
      duration_cast<seconds>(system_clock::now().time_since_epoch()).count();

  cout << "seconds since epoch: " << sec_since_epoch << endl;
  cout << "milliseconds since epoch: " << millisec_since_epoch << endl;

  return EXIT_SUCCESS;
}
```

输出：

```text
seconds since epoch: 1699499354
milliseconds since epoch: 1699499354951
```

## 使用符合POSIX标准风格的时间函数

[`gettimeofday`](https://www.ibm.com/docs/en/i/7.4?topic=ssw_ibm_i_74/apis/gettod.html)，符合 POSIX 标准，为C标准库方法，位于`<sys/time.h>` 头文件中(此方法只能在unix类系统中使用，[windows系统中需要寻求替代方法](https://www.codefull.org/2015/12/systime-h-replacement-for-windows/))。可以用于检索系统时钟，精确到微秒级。传递 `timeval`结构体类型变量的地址作为out参数来获取时间值。`tv_sec` 代表秒数，`tv_usec` 代表 微秒数。`gettimeofday` 返回 `int` 值 `0` 表示成功，`-1` 表示失败。函数的第二个参数是 `struct timezone`，通常只传递一个 `nullptr`。

```cpp
#include <sys/time.h>

#include <iostream>

int main() {
  	using std::cout,std::endl;
  
    struct timeval now;
    int rc;

    rc=gettimeofday(&now, nullptr);
    if(rc==0) {
        cout << "gettimeofday() successful." << endl;
    } else {
        cout << "gettimeofday() failed, errno = " << errno << endl;
        return -1;
    }
  
  	time_t ms_time = (now.tv_sec * 1000) + (now.tv_usec / 1000);

  	cout << "seconds since epoch: " << now.tv_sec << endl;
  	cout << "milliseconds since epoch: " << ms_time << endl << endl;

  	return EXIT_SUCCESS;
}

```

输出：

```text
gettimeofday() successful.
seconds since epoch: 1699498713
milliseconds since epoch: 1699498713690
```

## 一些易混淆的时间函数

https://zhuanlan.zhihu.com/p/152680520

https://www.toutiao.com/article/6700333137374741000/?wid=1699089324083

https://www.cnblogs.com/sunchaothu/p/10551344.html

steady_clock和system_clock的区别

1、`std::chrono::system_clock()::to_time_t(now)`

2、 std::chrono::time_point<std::chrono::steady_clock> time = std::chrono::steady_clock::now();
    auto timeInSeconds = std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count()

https://www.runoob.com/w3cnote/cpp-time_t.html

https://www.delftstack.com/zh/howto/cpp/how-to-get-time-in-milliseconds-cpp/





