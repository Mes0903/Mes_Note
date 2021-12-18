---
###### tags: `OS`
---

# Ch1 基礎概論  

## 什麼是作業系統?  

### Introduction  

「作業系統（英語：Operating System，縮寫：OS）是一組主管並控制電腦操作、運用和執行硬體、軟體資源和提供公共服務來組織使用者互動的相互關聯的系統軟體程式」── [wiki](https://zh.wikipedia.org/wiki/%E6%93%8D%E4%BD%9C%E7%B3%BB%E7%BB%9F)

簡單來說就是一個管理電腦的系統程式，是使用者和電腦硬體的介面 (interface)。

### Computer System

整個電腦系統主要可以分成四個部分：硬體、作業系統、應用程式和使用者，這邊先對它們做個簡單的介紹，但這些不是定義，只是一個描述

+ 使用者(User)
    
    使用者可以是人、機器或其他的電腦，只要是可以操控這台電腦的都可以算
    
+ 應用程式(Application)

    能幫助使用者解決問題的軟體都可以算，像是 Browser、Compiler 或一般的 Text Editor 都算
    
+ 硬體(Hardware)

    能夠拿來給我們操作，做運算的硬體，因此硬體在 OS 這邊我們習慣稱它們回資源 (resources)，我們不在意它是透過哪種硬體提供的，只在意它能夠提供哪種資源，可以怎麼運用它
    
+ 作業系統(Operating System)

    能夠幫助我們「控制(control)」和「協調(coordinate)」資源的系統軟體。
    
    控制(control) 指的大概就是 Device Driver，能夠幫助我們去控制硬體的，重點在協調(coordinate)，怎麼樣去協調使用者，分配資源，是 OS 裡面比較複雜的部份。
    
    因此我們也可以說 OS 是幫忙分配資源的軟體(resource allocator)，像是 memory management 會幫忙分配記憶體，file system 會幫忙分配 disk block 等等
    
    因此 OS 比較常見的定義是一個 resource allocator 和 control program，我們也稱它為 kernel，是一個在電腦內部隨時都在執行的程式，前面兩個名詞可以不用特定去記，因為只是一個比較常見的定義，但其實沒有很確切的定義 OS 是什麼，<span class="yellow">但 kernel 這個名詞很重要，要記一下</span>。

因此作業系統就會有一些 for virtual resource 的 API 可以使用，這些 API 我們也常稱它為 system call，<span class="yellow">這些 API 是 user 與 resource 間唯一的 interface</span>。

### Example

我們看一個例子：

![](https://i.imgur.com/buOkGjU.png)

最下面是硬體，上面則是我們寫的程式，程式大致上可以分成「和作業系統相關的」與「和作業系統無關的」，也就是圖上的 user mode 與 kernel mode，這在後面的章節會再提更多。

Device Driver 也算 OS 的一部份，把它抽出來是因為它是可以一直更新、抽換的；Driver 上面就會有 OS 的主要部分，也就是如何管理 memory、cpu 等等的軟體，這個軟體會提供一些 API，也就是前面提到的 System call，OS 會把它包起來成 System library，讓我們使用。

我們寫完 Program 後要先 Compile，然後透過 Linker 把這些 Compile 完的 `.o` 檔 link 在一起，代表我們要去使用別人 implement 好的 Program，因此他不會只 link 我們 include 的那些 library，也一定會 link System library，link 完後才成為一個能執行的執行檔。

以 `printf` 來舉例，我們要印出東西在終端機上，這個動作不會是 C library 去做，而是 OS 去做的，需要呼叫到 system call，可能不是直接呼叫，但一定間接會呼叫到它，所以一定要 link System library，等到後面講 system call 時會更清楚。

因此這個過程中我們會先呼叫 `printf`，然後會呼叫到 system call，再去呼叫到 driver 這樣一層一層下去。

### Goals of an Operating System

OS 主要的考量、需求有二：

1. 方便
    尤其是越接近使用者的產品，方便性越重要。

2. 效率
    如果執行一些比較複雜的程式，像是遊戲，或是你要算什麼東西的時候，就希望它效率會快一點，換句話說就是 performance 要好一點。

而這兩點是 trade-off 的，因為「方便」是給 user 的，一定會多加到一些東西，所以速度就會慢下來，因此就看怎麼設計怎麼取捨。

### Modern Operating Systems

這邊列幾個常見的 OS：

+ x86 platform

    + Linux
        像是 CentOS、Redhat、Ubuntu 等等的，大型的系統還是會選擇 Linux，主要是因為它是 open source 的。

    + Windows
        這就是一般使用者比較熟系的 OS。

+ PowerPC platform

    如 Mac OS。

+ Smartphone Mobile OS

    如 Android、iOS、Windows10 Mobile、Ubuntu Touch 等等。

+ Embedded OS (嵌入式系統)

    Embedded 的範圍非常廣泛，像是物聯網上面的 Device 形形色色，因此在上面的 OS 就需要非常有彈性，且能夠調整，但當然也有一些 OS 是綁在那個硬體上的。
    
    如 Raspberry Pi、Xbox 等等

## Computer-System Organization

知道了 OS 的定位、角色之後就要開始講它到底是如何運作的了。

電腦系統有很多不同的 Hardware，我們需要把它串起來，你把主機打開來可以看見它有 bus、線，而作業系統的目的就是要控制、協調這些硬體的使用，可以看看下面這張圖：

![](https://i.imgur.com/CXNZzw7.jpg)

簡單來說就會長上圖最下面那樣，執行程式時 instruction 是 run 在 cpu 上的，需要用到的 data 會在 memory 裡面，最後可能寫到某個 Device 上面，看要儲存還是輸出之類的，這些是 control 的部分。

而 coordinate 部份的問題是今天可能有很多程式，OS 需要讓它們能夠同時執行在這台電腦上而不出錯，像是 A Process 不能去修改到 B Process 的資料之類的。

### Computer-System Operations

這是一個 OS 基本運作的例子：

![](https://i.imgur.com/rTCjl1Q.jpg)

這邊 Cpu 想對一個 I/O Device，像是 Disk 做動作，例如讀寫資料。

如果 Cpu 想要把資料寫過去，那它會需要一個 Device Controller，這是一個硬體，它是一個最 low-level 能夠控制 Device(例如 Disk 的磁頭轉動) 的硬體，每個 Device Controller 會負責自己特定的 Device。

而因為資料需要在 Device Controller 上流動，因此 Device Controller 一定會有 buffer，資料會先寫到這上面再寫出去，因為 I/O Device 相較於 cpu 很慢，如果沒有這個 Buffer，代表我們要直接把資料從 Device 上 Copy 到 Memory 裡，這個需要很久，因此 cpu 一定會 idle。

而為了要透過 buffer 去讀寫，Device Controller 就會需要有 Status Register 和 Data Register 來控制 Device Controller。 Status Register 會告訴它現在是 busy 還是 idle；Data Register 則可以想像成另一個 buffer，我們先很快寫到 Register 上，再寫到 buffer。

因此在讀資料時，I/O Device 需要先和 Device Controller 做 I/O，跟 cpu 無關，但是後面因為 memory 是 cpu 在使用的，所以後半段的操作就需要 cpu 去下指令操作。

### Busy Waiting

那麼 cpu 那邊要怎麼控制呢? 一個早期大家想到的最簡單的方法就是 Busy Waiting，完全由 cpu 來控制，這邊舉個簡單的例子：

```c
#define OUT_CHAR 0x1000    // device data register
#define OUT_STATUS 0x1001    // device status register

current_char = mystring;
while ( *current_char != ‘\0’) {
    poke( OUT_CHAR, *current_char );
    while ( peek( OUT_STATUS ) != 0 );    // busy waiting
    current_char++;
}
```

這邊我們想要把一串字串寫到 I/O Device 上，因為 buffer 空間大小的關係，且 cpu、memory 那邊的速度遠快於 I/O Device 的速度，因此我們不可能一次把全部的資料塞進 buffer，或像 streaming 那樣資料不停的流動，所以 Busy Waiting 的方式就是，如果 buffer 寫滿了，就等到 buffer 寫到 Disk 上，清空後再寫下一批。

所以 cpu 這邊的 code

```c
while ( *current_char != ‘\0’)
```

是在判斷字串結束了沒，`poke` 是把值寫到 buffer，而我們在寫的時候，因為怕 buffer 還沒清空，是 busy 的狀態，所以會需要一個 while loop 去看 Status Register 的值：

```c
while ( peek( OUT_STATUS ) != 0 );    // busy waiting
```

如果是 busy 的，就停在那邊等，所以才叫 Busy Waiting；當值不等於 `0` 時，代表已經沒事了，就可以換下一個字了。

因此整個做 I/O 的時間並不是真正在寫的時間，中間有花一段時間在等 buffer 清空，也就是 cpu 有某種程度的 idle。

如果我們是這樣做 I/O 的，那整個效率就會很差，因為 I/O 和 cpu 並沒有 overlap，一個程式在做 I/O 的時候仍會霸佔住 cpu。

### Interrupt

因為 Busy waiting 非常沒有效率，在檢查是否為 busy 時 cpu 沒法做其他的事，會整個卡在那邊，也因此會很難同時處理多個 I/O。

所以就有另外一個方式叫做 Interupt，這是一種 event driven 的架構。

{%hackmd aPqG0f7uS3CSdeXvHSYQKQ %}