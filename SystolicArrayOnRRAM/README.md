{%hackmd aPqG0f7uS3CSdeXvHSYQKQ %}

# 前言

這學期(大四上) 修了一門課叫新興記憶儲存系統元件設計，主要在講一些現代 Non-volatile memory，如 FRAM 和 ReRAM，還有一些較新的 Disk 的設計，如 Open-channel SSD 和 ZNS 等等的

而這門課有專題，也就是需要找個題目研讀一下，雖然老師說不需要實作出來，找個還沒被解過的問題，然後試著提解法就好，但我是個喜歡實作的人，沒有實作出來就覺得哪裡怪怪的，所以就希望能找個實作來做

然而由於想要實作，所以要選「還沒被解過的題目」這件事變成了一個困難，首先我只有大概 2 個月的時間，要想出一個解法並實作他比單獨的理解後實作難很多，依照我的經驗，過程中如果遇到一些預期外的問題很容易找不到解法而卡住，之前有看到一篇好文可以供大家參考：[因為自動飲料機而延畢的那一年](https://hackmd.io/@jserv/HkyCt0hqb?type=view)

這只是一門課的專題，我這學期還有一堆事，Lab 那邊我們伺服器架構整個爛掉，我需要將整個架構重建(這又是另一個故事了)；我們的 core service 也在重構，加上我目前算是大家的 senior，需要盯緊底下學弟妹的 lab 專案進度；而我自己本身還有 OSDI 的專題、跟 pp 他們一起修的 Linux 課堂小專案、圖學課的小專案還有開給實驗室上的 C\+\+ 與 OS 課

這樣打出來發現事情真的很多，因此，雖然我本身蠻有興趣的，但我沒有辦法把全部的時間花在這個專案上，上上週 pp 很罩的只花了一兩天就把 Linux kernel 的第一個小專案做完了，我跟洪祥銘只需要做報告就好，結果我到現在竟然都還沒有空動筆，希望記憶體的期中報告完後可以把它趕快寫完，太對不起 pp 了 QQ

基於這些理由，我的想法很簡單，找一些別人還沒有在 Non-volatile memory 這塊做過的演算法，我用 Non-volatile memory 做一些 in-memory computing 看看，這樣一來演算法不會太難，而且會有在別的地方做過的相關論文可以參考，這大概也勉強算在「還沒被解過」的範疇內

而很剛好的，張家華學長對生物工程這方面有所涉略，真的是太猛了，我不管跑去玩什麼有的沒的，他好像都可以給我建議，真的是大神。總之，學長看了看說可以考慮把 systolic array 加進來，因為 systolic array 上面每個 cell 都是一個 memory unit，而我們又能做 in-memory computing，理論上可以做得到，而且這樣可以讓題目更落在「還沒被解過」的圈圈裡面

因此和老師和學長討論後，選的演算法就是 Smith-Waterman Algorithm 了，演算法不會太 hardcode，但實作部分我就不知道了，學長也沒碰過 ReRAM，也許之後會常常去敲老師，和我的大腿組員趙昱平，還有發科的大神學長天澤大大

整個實作可以被分為兩部分：

1. 利用 systolic array 實作 Smith-waterman Algorithm
2. 利用 ReRAM 實作 systolic array

而依照老師的建議，會使用 gem5 + NVMain 來進行模擬，google 搜了一下感覺這東西光是 build 起來就有一堆點要注意，而且目前還沒看到有哪個教學有寫說該怎麼使用，希望不要搞一搞到最後發現需要寫 HDL 和 hspice，這兩個我只有修過一學期不到的課，實在很不熟，如果用到的話還要再額外去讀，時間不知道夠不夠用，感覺起來困難重重啊(汗

Smith-Waterman Algorithm 是一個用在生物工程的演算法，由 Needleman-Wunsch 變化而來，用來做基因序列的比對，所以我們這篇文就從基因序列比對開始介紹起吧~

# 基因序列比對

這一段介紹轉載自[基因序列比對演算法](https://hackmd.io/@UlvydjoQQKafa4iwBkJ9lg/BkGumVZtu)，寫得很好，我只小改了一些用詞，有興趣的可以去看看原文

一般而言，我們會把 DNA 和蛋白質分別看成是由 4 和 20 個英文字母所組成的序列或字串，因為他們分別是由 4 種核苷酸和 20 種胺基酸所組成的。對 DNA 而言，突變是非常平常的事情，也是自然的演化過程。藉由基因的突變，生物可以適應自然環境的改變。

## DNA突變的類型

常見的DNA突變有3種，分別是取代、插入及刪除。

| DNA突變行為 | 意義                                          |
| ----------- | --------------------------------------------- |
| 取代(配錯)  | 把一個字母用另一個字母取代                    | 
| 插入        | 在 DNA 序列的某一個位置插入一個字母 |
| 刪除        | 在 DNA 序列的某一個位置刪除一個字母 |

## 編輯距離

通常生物學家會利用所謂的**編輯距離**，來衡量兩條 DNA 序列之間的相異程度。生命總是朝著最短路徑進行演化，所以兩條序列之間的編輯距離被定義為：把其中一條序列編輯轉成另外一條序列，所需最少的編輯運算個數。兩條 DNA 序列之間的編輯距離越小，代表它們之間的相似程度越高。從演化的觀點來說，這意味著它們演化自同一個祖先（即所謂的同源），所以彼此間應該會有相似的結構及功能。

生物學家喜歡利用比對來衡量兩條序列之間的相似程度。拿 `GACGGATAG` 和 `GATCGGAATAG` 這兩條 DNA 序列來說，乍看之下這兩條長度不同的DNA序列似乎不太相似。但當我們如下表一般，把它們重疊在一起，並在第 1 條序列的第 2 個和第 3 個字母間，與第 6 個和第 7 個字母之間分別插入一個空白字，就可發現其實這兩條 DNA 序列還蠻相像的。

| 重疊前      | 重疊後      |
| ----------- | ----------- |
| GACGGATAG   | GA_CGGA_TAG |
| GATCGGAATAG | GATCGGAATAG |

這種將序列重疊在一起的方式，就稱為序列的比對

我們可以在兩條序列的任意位置上插入一個或多個空白字，這樣做的目的是讓相同或相似的字母能夠儘量對齊。但要特別注意的是，不能讓兩個插入的空白字對齊在一起，因為這樣對衡量序列之間的相似程度並無幫助。

因此字母之間對齊的方式其實就只有2種:

1. 字母與字母的對齊
2. 字母與空白字的對齊

# 對齊(Alignment)

我們可以把 Alignment 的演算法分為 Global 與 Local 兩種

+ Global Alignment（全局比對）：
    + 目標： 在整個序列的整體範圍內找到最佳的對應，強調整個序列的相似性
    + 特點： 將整個序列進行比對，並試圖找到一個最佳的比對方案，即最大相似性得分。這種比對通常用於比較兩個相對較相似的序列，如同一基因在不同物種中的同源基因
    + 應用： 常見的全局比對算法包括 Needleman-Wunsch 算法
    
+ Local Alignment（局部比對）：
    + 目標： 在序列的某個區域內找到最佳的對應，強調局部相似性
    + 特點： 局部比對專注於找到序列中的局部相似區域，而不要求整個序列的相似性。這種比對通常用於比較兩個相對較不相似的序列，以尋找局部相似的區域，如同源基因的某一片段
    + 應用： 常見的局部比對算法包括 Smith-Waterman 算法

## Needleman-Wunsch

### 步驟

Needleman-Wunsch 的步驟如下

1. 初始化得分矩陣（Initialization）：
    給予兩個序列 $M$ 與 $N$，創建一個矩陣，大小為 $(m+1) \times (n+1)$，其中 $m$ 和 $n$ 分別是兩個輸入序列的長度。
    
    接下來初始化第一行和第一列的值，使第一列的每個元素等於 $0 - gap\ penalty \times 行號$，每一行的每個元素等於 $0 - gap\ penalty \times 列號$，其中 gap penalty 是插入或刪除的懲罰分數
    
2. 計算得分（Matrix Filling）：
    遍歷得分矩陣的每個元素 $F(i,j)$，計算其得分。 得分是由三個可能的來源數值選出的最大值，這三個來源數值為：
    + $F(i-1, j-1) + match/mismatch\ score$：
        來自左上方的對角線的元素，表示匹配或不匹配的得分
    + $F(i-1, j) - gap\ penalty$：
        來自上方的元素，表示插入的得分
    + $F(i, j-1) - gap\ penalty$：
        來自左方的元素，表示刪除的得分
3. 回溯（Traceback）：
    從右下角開始回溯，如果 $M_i = N_j$，則回溯到左上角那格，如果 $M_i \neq N_j$，則回溯到左上角、上面、左邊中值最大的那格，其中 $M_i$ 表示 $M$ 內的第 $i$ 個元素，$N_j$ 同理；如果有相同最大值的格子，按照左上角、上面、左邊的順序來回溯
    
4. 寫出回溯路徑
    最後我們就根據回溯路徑，寫出匹配的序列了，假設輸出的序列為 $M'$ 與 $N'$：
    + 如果回溯到左上角的格子，將 $M_i$ 添加到 $M'$ 中，將 $N_j$ 添加到 $N'$ 中
    + 如果回溯到上面的格子，將 $M_i$ 添加到 $M'$，將空格 `_` 添加到 $N'$ 中
    + 如果回溯到左邊的格子，將空格 `_` 添加到 $M'$ 中，將 $N_j$ 添加到 $N'$ 中

### 例子

這邊給一個例子，我們考慮兩個 sequence：

```mathematica
N: ATGCT
M: AGCT
```

然後設定匹配得分(Match) 為 +1，不匹配懲罰分數(Mismatch) 為 -1，插入/刪除的懲罰分數(GAP) 為 -2，因此得分矩陣（Score Matrix）初始化的樣子如下：

```css
   -   A   T   G   C   T -- 序列 N
-  0  -2  -4  -6  -8 -10 -- 初始化為 0 - (-2) * i-th column
A -2
G -4
C -6
T -8
|  |--初始化為 0 - (-2) * j-th row
|--序列 M
```

然後填充，我們以 $F(2,2)$ 為例子計算一次，我這裡 index 從 $1$ 開始，$F(1,1)$ 到 $F(2,2)$ 的局部樣子為：

```css
   -   A
-  0  -2
A -2   ?
```

因此三個來源數值為：

+ 左上方：$0 + 1 = 1$，因此為 $1$
    這邊是 $+1$ 而不是 $+(-1)$ 的原因是因為 $A == A$，因此為 match，加上 match score
+ 上方：$(-2) - 2 = -4$，因此為 $-4$
+ 左方：$(-2) - 2 = -4$，因此為 $-4$

三個數值中最大的數值為 $1$，因此填入 $1$：

```css
   -   A
-  0  -2
A -2   1
```

接下來我們往右一個元素繼續執行這個過程，$F(1,1)$ 到 $F(2,3)$ 的局部樣子為：

```css
   -   A   T
-  0  -2  -4
A -2   1   ?
```

三個來源數值為：

+ 左上方：$(-2) + (-1) = -3$，因此為 $-3$
    這邊是 $+ (-1)$ 而不是 $+1$ 的原因是因為 $A \neq T$，因此為 mismatch，加上 mismatch score
+ 上方：$(-4) - 2 = -6$，因此為 $-6$
+ 左方：$1 - 2 = -1$，因此為 $-1$

三個數值中最大的為 $-1$，因此填入 $-1$：

```css
   -   A   T
-  0  -2  -4
A -2   1  -1
```

對每一層持續進行這個過程，第一次完成後矩陣的樣子為：

```css
   -   A   T   G   C   T
-  0  -2  -4  -6  -8 -10
A -2   1
G -4    
C -6    
T -8   
```

第二次迭代：

```css
   -   A   T   G   C   T
-  0  -2  -4  -6  -8 -10
A -2   1  -1
G -4  -1 
C -6  
T -8  
```

整個流程完成後，矩陣的樣子為：

```css
   -   A   T   G   C   T
-  0  -2  -4  -6  -8 -10
A -2   1  -1  -3  -5  -7
G -4  -1   0   0  -2  -4
C -6  -3  -2  -1   1  -1
T -8  -5  -2  -3  -1   2
```

Score Matrix 填充完畢後，開始從右下角回溯，找到最佳路徑。

由 $F(5, 6)$ 的 $2$ 開始，由於 $C == C$，所以往左上角走，接下來 $G == G$，因此再往左上角走，接下來 $A \neq T$，因此找最大的格子，為 $F(2,2)$ 的 $1$，往左走，然後因為 $A == A$，因此再往左上角走

因此整體路徑為：$F(5,6) => F(4,5) => F(3,4) => F(3, 3) => F(2, 2)$
方向的話為：$左上 => 左上 => 左 => 左上$

```mathematica
N': ATGCT
M': A-GCT
```

## Smith-Waterman

與全局比對算法（如 Needleman-Wunsch）不同，Smith-Waterman 專注於找到序列中的局部相似區域，並計算這些區域的最大得分

### 步驟

Smith-Waterman 的步驟如下：

1. 初始化得分矩陣（Initialization）：
    給予兩個序列 $M$ 與 $N$，創建一個矩陣，大小為 $(m+1) \times (n+1)$，其中 $m$ 和 $n$ 分別是兩個輸入序列的長度
    
    接下來初始化第一行和第一列的值初始化為 $0$
    
2. 計算得分（Matrix Filling）：
    遍歷得分矩陣的每個元素 $F(i,j)$，計算其得分。 得分是由三個可能的來源數值選出的最大值，這三個來源數值為：
    + 如果這三個數值的最大值小於 $0$，則取 $0$
    + $F(i-1, j-1) + match/mismatch\ score$：
        來自左上方的對角線的元素，表示匹配或不匹配的得分
    + $F(i-1, j) - gap\ penalty$：
        來自上方的元素，表示插入的得分
    + $F(i, j-1) - gap\ penalty$：
        來自左方的元素，表示刪除的得分
        
3. 找到最大得分
    在填充矩陣的過程中將最大值的值與位置記錄起來
    
4. 回溯（Traceback）：
    從最大值的位置開始回溯，如果 $M_i = N_j$，則回溯到左上角那格，如果 $M_i \neq N_j$，則回溯到左上角、上面、左邊中值最大的那格，其中 $M_i$ 表示 $M$ 內的第 $i$ 個元素，$N_j$ 同理；如果有相同最大值的格子，按照左上角、上面、左邊的順序來回溯
    
5. 寫出回溯路徑
    最後我們就根據回溯路徑，寫出匹配的序列了，假設輸出的序列為 $M'$ 與 $N'$：
    + 如果回溯到左上角的格子，將 $M_i$ 添加到 $M'$ 中，將 $N_j$ 添加到 $N'$ 中
    + 如果回溯到上面的格子，將 $M_i$ 添加到 $M'$，將空格 `_` 添加到 $N'$ 中
    + 如果回溯到左邊的格子，將空格 `_` 添加到 $M'$ 中，將 $N_j$ 添加到 $N'$ 中

### 例子

這邊我使用一篇 [CSDN](https://blog.csdn.net/yohjob/article/details/89144032) 裡面的例子，我們考慮兩個 sequence：

```mathematica
N: TGTTACGG
M: GGTTGACTA
```

然後設定匹配得分(Match) 為 +3，不匹配懲罰分數(Mismatch) 為 -3，插入/刪除的懲罰分數(GAP) 為 -2

首先將得分矩陣（Score Matrix）初始化：

```css
   -   T   G   T   T   A   C   G   G -- 序列 N
-  0   0   0   0   0   0   0   0   0
G  0
G  0
T  0
T  0
G  0
A  0
C  0
T  0
A  0
|--序列 M
```

然後填充，我們以 $F(2,2)$ 為例子計算一次，我這裡 index 從 $1$ 開始，$F(1,1)$ 到 $F(2,2)$ 的局部樣子為：

```css
   -   T
-  0   0
G  0   ?
```

因此三個來源數值為：

+ 左上方：$0 + (-3) = -3$，因此為 $-3$
+ 上方：$0 - 2 = -2$，因此為 $-2$
+ 左方：$0 - 2 = -2$，因此為 $-2$

由於三個數值中最大的為 $-2$，<span class = "yellow">小於 $0$</span>，因此填入 $0$：

```css
   -   T
-  0   0
G  0   0
```

接下來我們往右一個元素繼續執行這個過程，$F(1,1)$ 到 $F(2,3)$ 的局部樣子為：

```css
   -   T   G
-  0   0   0
G  0   0   ?
```

三個來源數值為：

+ 左上方：$0 + 3 = 3$，因此為 $3$
+ 上方：$0 - 2 = -2$，因此為 $-2$
+ 左方：$0 - 2 = -1$，因此為 $-2$

由於三個數值中最大的為 $3$，因此填入 $3$：

```css
   -   A   T
-  0   0   0
A  0   0   3
```

對每一層持續進行這個過程，第一次完成後矩陣的樣子為：

```css
   -   T   G   T   T   A   C   G   G 
-  0   0   0   0   0   0   0   0   0
G  0   0   
G  0  
T  0  
T  0  
G  0  
A  0  
C  0  
T  0  
A  0  
```

第二次迭代：

```css
   -   T   G   T   T   A   C   G   G 
-  0   0   0   0   0   0   0   0   0
G  0   0   3   
G  0   0   
T  0   
T  0   
G  0   
A  0   
C  0   
T  0   
A  0   
```

整個流程完成後，矩陣的樣子為：

```css
   -   T   G   T   T   A   C   G   G 
-  0   0   0   0   0   0   0   0   0
G  0   0   3   1   0   0   0   3   3
G  0   0   3   1   0   0   0   3   6
T  0   3   1   6   4   2   0   1   4
T  0   3   1   4   9   7   5   3   2
G  0   1   6   4   7   6   4   8   6
A  0   0   4   3   5  10   8   6   5
C  0   0   2   1   3   8  13  11   9
T  0   3   1   5   4   6  11  10   8
A  0   1   0   3   2   7   9   8   7
```

Score Matrix 填充完畢後，開始從右下角回溯，找到最佳路徑。

最大值為 13，其位於 $F(8,7)$，因此從 $F(8,7)$ 開始回溯

回溯路徑如下圖：

<center>    
<img src="https://hackmd.io/_uploads/S1RAWmZ4T.png">

圖源：[Smith-Waterman算法、Needleman-Wunsch算法的算法原理及算法比较](https://blog.csdn.net/yohjob/article/details/89144032)

</center>

因此最終得到的序列為

```mathematica
N': GTT_AC
M': GTTGAC
```

# ReRAM (RRAM)

ReRAM 是一種新型的非揮發性記憶體，所謂的「非揮發性」表示斷電後 memory 內的數據並不會消失，會被保存下來，跟目前主流的記憶體不一樣

具我所知目前 ReRAM 還沒有終端產品，ReRAM 基於內部的可變電阻，可以通過改變 memory cell 內的的電阻來實現 in-memory computing，而至於內部怎麼實現，那些太材料的東西我也不會，所以這邊就不多闡述

至於內部電路原理，其會有一個

有一篇知乎寫得還不錯，但因為他禁止轉載，所以連結放這邊建議大家可以點進去看看：[閱讀筆記一：RRAM (ReRAM)](https://zhuanlan.zhihu.com/p/35152668?fbclid=IwAR1thNfaXP8UjS_mte_qyCicaqOGAc85KyIaIlT80i7vEFR9wpgiRVDg920)

(或是等我看完原文論文再回來寫這段XD)

# Systolic Array

這邊使用一下清大林永隆教授的數邏課堂上的內容來介紹 Systolic Array，建議大家也可以去看看上課影片：

+ [19a 數位邏輯設計 Systolic Array](https://www.youtube.com/watch?v=6PFgOMHAUo0)
+ [19b 數位邏輯設計 Systolic Array](https://www.youtube.com/watch?v=oswGZwb5Dcw)

Systolic Array 是由孔祥重院士提出的，問題的起因是把東西從 cache 搬到 register 的開銷其實是實際進行運算的開銷的好幾倍，若我們搬完一筆數據後只拿去運算一次就移走他，之後用到他的時候才又把它搬回來，就會造成另一筆額外且相對大的開銷

所以核心的思想就是讓資料搬進 register 後要好好的被利用，做很多很多的運算，做完後才把數據移走

下圖中的「PE」是運算單元，可以看見其將數據一次性地經過了多個 PE：

<center>
    
<img src="https://hackmd.io/_uploads/rJwygdG4a.png">

圖源：Kung, "Why systolic architectures?," in Computer, vol. 15, no. 1, pp. 37-46, Jan. 1982, doi: 10.1109/MC.1982.1653825.
    
</center>

所以你可以猜到並不是所有的運算都適合利用 systolic array 來運算，因為數據並不一定符合「能一次性地做很多很多的運算」的這個特性

### 例子 1

那麼有什麼樣的運算符合呢? 在孔祥重院士的論文中舉了一個例子：捲積(Convolution)

我們先從一維的開始下手，Convolution 是現代很常使用到的計算方法，在 Convolution 的運算中會有兩個輸入序列，這邊假設輸入為 $X$ 與 $w$，其值為：

$$
\begin{cases}
X: \{3, 2, -1, 2, 0, 1\}\\
w:\{2, -1, 2\}
\end{cases}
$$

並且 Convolution 會有一個輸出序列，這邊假設輸出為 $Y$，其運算方式為 $\Sigma_{j=1}^3w_jX_{i+j-1}$，此處 Sigma 內的 `3` 為 $w$ 序列的元素個數

因此 $Y$ 的第一個元素為 $w_1X_1 + w_2X_2 + w_3X_3$，也就是 $w$ 與 $X$ 的前三個元素做內積，將值代入的話為：$(2\times 3)\ + (-1 \times 2)\ + (2 \times -1) = 2$。

按照這個方式計算過後，$Y$ 序列最終為：

$$
Y: \{2, 9, -4, 6\}
$$

此時孔院士就發現這可以有很多種算法，首先我們可以讓 $X$ 從我們元件的左邊按照設定好的順序傳進來，$w$ 的每個元素固定從元件的上方傳進來，每個元件內會有 register 與乘法器，我們將元素依序在每個元件內做相乘並存起來

假設一個開始 `t = 0`，則當 `t = 3` 時整個架構會長得像這樣：

<center><img src="https://hackmd.io/_uploads/SJj_n_GEa.png"></center><br>

上圖中有三個 PE，每個 PE 內有一個 register 用來存進來的 $X_i$，而每個 PE 的上方會有固定的 $w_i$ 數據傳入，PE 的內部有一個乘法器負責做乘法運算，運算完的結果會傳出來跟其他 PE 的運算結果做加法，整個做完後就是我們要的 $Y_1$ 了

可以再多看一個 $Y_2$ 的樣子：

<center><img src="https://hackmd.io/_uploads/H1sMyYMVT.png"></center><br>

而還有其他的方法，如孔院士提出的 broadcast inputs, move results, weights stay：

<center>
    
<img src="https://hackmd.io/_uploads/Sy_SlYfNp.png">

圖源：Kung, "Why systolic architectures?," in Computer, vol. 15, no. 1, pp. 37-46, Jan. 1982, doi: 10.1109/MC.1982.1653825.
    
</center>

在上例中，$w_i$ 會預先存在每一個 PE 裡面，$X_i$ 則從上方平行傳入對應的 PE 中，這被稱為「廣播」，而 $Y_i$ 則向右側依序傳入，這個動作被稱為「脈動」

從上例你也可以很簡單的看出到 `t = 3` 的時候，$Y_1$ 的值也會出來了

這樣的電路架構我們稱它為 Systolic Array，你可以看見它的結構非常的簡單且 Regular，每個 PE 做的操作都一樣，操作也不複雜，然而也因此導致其 Systolic Array 泛用性不高，很難同一組 Systolic Array 拿去做不同場合的運算，需要針對不同的場合設計不同的 Systolic Array

### 例子 2

接下來我們看一下二維的例子：矩陣運算

這也是 Google TPU 的應用，拿來做 ML 加速

假設我們現在有兩個 Array 做相乘，寫作 $C = A \times B$，其中 $A$、$B$、$C$ 都是矩陣，假設 $A$、$B$ 兩矩陣為：

$$
A =
\begin{bmatrix}
 1 & 2 \\
 3 & 4 \\
\end{bmatrix},\ 
B =
\begin{bmatrix}
 5 & 6 \\
 7 & 8 \\
\end{bmatrix}
$$

則

$$
C =
\begin{bmatrix}
 1 & 2 \\
 3 & 4 \\
\end{bmatrix}
\begin{bmatrix}
 5 & 6 \\
 7 & 8 \\
\end{bmatrix}=
\begin{bmatrix}
 19 & 22 \\
 43 & 50 \\
\end{bmatrix} 
$$

這個的 Systolic Array 會有四個 PE，如下圖所示：




# 

<center><img src=""></center><br>
<center><img src=""></center><br>
<center><img src=""></center><br>
<center><img src=""></center><br>
<center><img src=""></center><br>



# 參考資料

1. [基因序列比對演算法](https://hackmd.io/@UlvydjoQQKafa4iwBkJ9lg/BkGumVZtu)
2. [Global alignment vs. Local alignment vs. Semi-global alignment](https://bio.libretexts.org/Bookshelves/Computational_Biology/Book%3A_Computational_Biology_-_Genomes_Networks_and_Evolution_(Kellis_et_al.)/03%3A_Rapid_Sequence_Alignment_and_Database_Search/3.03%3A_Global_alignment_vs._Local_alignment_vs._Semi-global_alignment)
3. [Global Sequence Alignment & Needleman-Wunsch || Algorithm and Example](https://www.youtube.com/watch?v=ipp-pNRIp4g)
4. [Smith-Waterman算法、Needleman-Wunsch算法的算法原理及算法比较](https://blog.csdn.net/yohjob/article/details/89144032)
5. [閱讀筆記一：RRAM (ReRAM)](https://zhuanlan.zhihu.com/p/35152668?fbclid=IwAR1thNfaXP8UjS_mte_qyCicaqOGAc85KyIaIlT80i7vEFR9wpgiRVDg920)
6. [Kung, "Why systolic architectures?," in Computer, vol. 15, no. 1, pp. 37-46, Jan. 1982, doi: 10.1109/MC.1982.1653825.](https://ieeexplore.ieee.org/document/1653825)
7. [脈動陣列- 因Google TPU獲得新生](https://zhuanlan.zhihu.com/p/26522315)


