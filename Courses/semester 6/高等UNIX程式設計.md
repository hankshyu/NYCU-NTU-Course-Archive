# 高等UNIX程式設計
## 概述
- 課程名稱：高等UNIX程式設計
- 選修年度：110下
- 授課教師：黃俊穎老師
- 開課單位：資科工碩    
- 永久課號：IOC5214
- 學分數：3.00
- 必/選修：選修

如果你上完了作業系統，又在課程加退選的網站上看到這堂課的名字，你可以毫不猶豫地選下去<br>
如果你正在尋找一門課，幫助你了解 Unix / Linux，那你一定不要錯過<br>
如果你擔心專業選修課程給分很嚴格，害怕拉低學期分數，你更無須擔心

這堂課在選課階段就直接爆滿，第一，第二階段選課都沒能抽到他。最後在加退選階段寫信給老師加簽，透過抽籤的方式取得名額，可謂一波三折。在網路上看到很多有關於這堂課的心得，內容無不闡述他的特別。而我也決定親身體會這堂課的魅力，一堂資工系大學生跟碩士生搶成一團的課。

## 上課模式

課程的規劃緊貼教課書:

> Advanced Programming in the UNIX Environment, W. Richard Stevens, Stephen A. Rago

除了 單元8 (Assembly language integration) & 單元9 (ptrace and applications) 是老師自己補充的之外。基本上老師上課的單元與教科書中的單元是相互對應的，雖然說我有購買原文教科書，不過我認為大可不必，老師上課就已經講得很清楚了。

受到疫情的影響，這個學期老師都採用線上上課，實體考試的模式。老師會把三個小時的課程，都放在同一天，從 10:00 直接上到 13:00 左右。我有好幾次都是一邊吃奶油廚房的咖哩飯(感謝室友們)，一邊上課。線上課程結束之後也會將上課錄影上傳至課程網站，讓同學們可以回去複習。不過直到學期末，準時進到 Google Meet 平台上課的人數依然超過一半，可見上這堂課的同學都蠻認真，也跟老師優質的教學品質脫不了關係。老師的聲音非常清楚，有韻律起伏的同時又有些波瀾不驚，如果陷入老師的節奏，不知不覺三個小時就過去了，非常神奇。老師依使用投影片上課，過程中時常會現場演示給同學們看。我覺得這也是這堂課最大的賣點，老師每次的現場示範都以實際的操作在同學的腦中留下印象，對學習很有幫助。

作業的部分很紮實，整個學期總共有四次作業。前面的兩次比較簡單，利用一些老師上的提到的觀念寫程式。屬於花點時間就可以搞定
的類型。後面兩次題目就相較靈活，需要同學們自己找一些資歷或看 Linux 的原始碼來獲取靈感。老師也有設計一些 x86 Assembly 以及 ptrace 的練習放在網站上供同學練手，題目的難度都不困難，老師甚至課堂上還會給予提示。做完之後還可以依照完成的比例加學期總成績，非常划算，想拿高分務必要完成。採用類似 Online Judge 的系統，可以看到其他同學的分數，激發你的答題慾望。

考試採用到電腦教室上機考的形式，助教會給每個人一組帳號密碼，再用 ssh 連上助教提供的服務回答問題。不得不吐槽的是助教們似乎沒有把硬體的部分處理好，考試開始了一個多小時都還有超過一半以上的人連不進去。還蠻掉漆的。幸好在助教們的一通操作之下，考試開始後一個小時左右成功讓所有人連進系統。也將考試時間延長了一個小時，從三個小時變成四個小時，直接從 10:00 ~ 14:00 完試之後拖著疲憊的身軀走出系館。身體的疲憊加上困難考題的雙重夾攻，全身都快虛脫了，還要擔心午餐是不是都要賣光了，幸好新竹美食麥當勞還有開，感謝老天。

更慘的是我在乘機共步之後被老師以信件的方式約談，說我登入的帳號有一些問題，導致我的成績沒有被登入。我跟老師在辦公室裏面陳述完當時混論的情況後，老師就要我把所有寫出來的問題再寫一遍給他看(用老師的 Macbook Air 2016, 鍵盤很好打 XD)。老師在看完我的解題情況之後也有把我的分數還給我，希望學弟妹在上機考的時候重複確認自己登入的帳號密碼是否合理，才不會想我一樣被老師抓去Code Review 一遍，雖然我不是很緊張，還覺得經驗很特別就是了。

另外設備方面，老師上課會使用到類 Unix 的環境，所以如果有機會的話用 Mac 會方便很多，如果你是 PC 的話就要裝 Docker 或虛擬機，只要照者老師第一堂課的教學安裝應該就能成功裝起來，如果你是那種怕麻煩的人可以弄一台蘋果電腦會節省妳很多時間。 還有另一種方法就是申請資工系的工作站，或是連上老師位同學們準備的工作站(AMD Threadripper 32 Core)。但這些都沒有Root的權限，有些不方便，另外有少數實驗只能在本地端做，用自己的設備架設環境還是方便一點。

## 單元列表

Chap. | Topic |Slides 
:--------:|:----- |:---
1 |Fundamental tools and shell programming| [slide01][sl01]
2 |Files and directories| [slide04][sl04] 
3 |File I/O and standard I/O|[slide03+05][sl03+05]
4 |System data files and information|[slide04][sl04] [slide06][sl06]
5 |Process environment|[slide07][sl07]
6 |Process control|[slide08][sl08]
7 |Signals|[slide10][sl10]
8 |Assembly language integration|[slide10.5][sl10.5]
9 |ptrace and applications|[slide10.6][sl10.6]
10 |Threads|[slide11][sl11] [slide12][sl12]
11 |Inter-process communication|[slide15][sl15]
12 |Advanced IPC (Unix domain sockets)|[slide17][sl17]

期中考的範圍為 1 ~ 6，期末考則為 7 ~ 12。



## Labs 
Lab| Spec(PDF) |Brief description |Keywords
:---:|:-----:|:-----|:---
[Lab1][l1]|[⚙️][s1]|A 'lsof'-like program |[FILE I/O][sl04] [Files and directories][sl04] [System data files][sl06]
[Lab2][l2]|[⚙️][s2]|Logger program that can show file-access-related activities of an arbitrary binary|[Process environment][sl07] [Process control][sl08]
[Lab3][l3]|[⚙️][s3]|Extend a mini C library to support signal relevant system calls in x86 Assembly| [x86 Assembly][sl10] [Process control][sl08]
[Lab4][l4]|[⚙️][s4]|Scriptable Instruction Level Debugger|[Signals][sl10] [ptrace][sl12]

[sl01]:Slides/01-ov%2Btools.pdf
[sl04]:Slides/04-file%2Bdir.pdf
[sl03+05]:Slides/03%2B05-file%2Bstdio.pdf
[sl04]:Slides/04-file%2Bdir.pdf
[sl06]:Slides/06-sysinfo.pdf
[sl07]:Slides/07-procenv.pdf
[sl08]:Slides/08-procctrl.pdf
[sl10]:Slides/10-signals.pdf
[sl10.5]:Slides/10.5-assembly.pdf
[sl10.6]:Slides/10.6-ptrace.pdf
[sl11]:Slides/11-threads.pdf
[sl12]:Slides/12-threadctrl.pdf
[sl15]:Slides/15-classipc.pdf
[sl17]:Slides/17-advipc.pdf
  
[s1]:HW1/unix_hw1.pdf
[s2]:HW2/unix_hw2.pdf
[s3]:HW3/unix_hw3.pdf
[s4]:HW4/unix_hw4.pdf


[l1]:HW1/
[l2]:HW2/
[l3]:HW3/
[l4]:HW4/



## 評分方式

#### 成績分布

分數 | HW1 | HW2 |HW3 | HW4 | 期中考 | 學期成績
:------:|:-----:|:---:|:---:|:---:|:---:|:---:|
0∼ 59分     | 32    |3     |24   | 24    |0     |22  
60∼ 69分    | 4 　  |3     |0   | 6    |26     |4 
70∼ 79分    | 4 　 |9     |6   | 18    |42     |9  
80∼ 89分    | 5　  |22    |4  | 34    |17     |31  
90~ 100分   |　76 　|63    |87   | 39   |18     |55  

在學期初公布的配分比例為 

- 作業(40%)
- 期中考(25%)
- 期末考(35%)

但由於學校針對疫情採取遠距上課的措施，我們這屆的期末考被取消，並以四次作業的加權平均填補，比例是:

- HW1(10%)
- HW2(10%)
- HW3(30%)
- HW4(50%)

因為新政策出台的時候只剩下 HW4 的期限還沒有到，再加上作業三與作業四的難度比較高，老師決定提高兩者的比例。最後的學期分數意外的蠻甜的，超級多人都有拿到A+，或許是因為期末考被取消的緣故吧!大家也只能在寫功課的時候認真一點。

## 結語

這是我覺得很符合交大資工的一堂課: 名氣很響亮，同儕很厲害，內容很紮實，給分也很大方(如果你有認真的話)。<br>
不過對於程式能力會有一定的要求(C Programming) 。強烈推薦給所有的人，如果有機會上這堂課的話千萬不要錯過!

