# 數位訊號處理架構設計
## 課程基本資訊

- 開課學期：112-1
- 授課老師：簡韶逸
- 課號：EE 5141
- 課程辨識碼：921 U9330
- 學分：3
- 必/選修：選修

就如同巧克力蛋糕的主體是蛋糕一樣，這堂課的主體不是數位訊號處理，而是探索各種硬體架構設計技術。如果在其他電路設計課程中帶給同學的如何從“電路”的層級得到一個 PPA 均衡的設計，那這一堂課會帶領同學從更高層級的架構出發探索。課程中會使用一些間單的數學模型像是圖和樹之類的資料結構來探討架構設計的問題，我認為很有趣也很實用。如果能把這堂課所教授的技能都融會貫通，並且在設計電路的時候應用出來，那相信你一定可以和其他選手拉開一大段差距。

## 上課模式

課程圍繞著教科書

> K. K. Parhi, VLSI Digital Signal Processing Systems, John Wiley & Sons, 1999.

基本上前九個單元都可以在書上找到相對應的單元。老師上課會使用投影片做講義，算是把課本內容的重點都擷取下來。我個人覺得做得非常好，基本上可以說是到了不用看原文書也沒關係的地步了。其實書上出的錯還蠻多的，老師也會在課堂上只是同學別被課本錯的內容誤導。這本數是一位很厲害的教授出版的，她把很多電路設計的問題轉換為數學問題，算是在這個領域有非常傑出的貢獻。老師上課的方式是在 NTU Cool 上面放過去上課的影片，每個單元介於45分鐘到一個半小時之間，算是非常有效率。令過每隔兩三個禮拜老師也會公布進行一堂實體課，覺得蠻好的，可以自由分配時間。課堂上就是把累積兩週的投影片進行重點提點。不知道是不是所有同學都喜歡這種模式呢？我個人是蠻喜歡自由分配時間的，因此大推！！


## 單元列表

|  Chap.   | Topic  |   Slides   | H.W.  | H.W. Ans.   | H.W. Avg.  |
|  ----  | ----  | :----:  | :----:  | :----:  | ----  |
1 |Introduction| [📝][sl01] | [✏️][hw01] | [😊][ans01] | 99.314 |
2 |Iteration bound| [📝][sl02] | [✏️][hw02] | [😊][ans02] | 98.358 | 
3 |Pipelining and Parallel Processing|[📝][sl03] | [✏️][hw03] | [😊][ans03] | 95.817 |
4 |Retiming|[📝][sl04] | [✏️][hw04] | [😊][ans04] | 97.424 |
5 |Unfolding|[📝][sl05] | [✏️][hw05] | [😊][ans05] | 96.419 |
6 |Folding|[📝][sl06] | [✏️][hw06] | [😊][ans06] | 91.847 |
7 |Systollic Architecture Design|[📝][sl07] | [✏️][hw07] | [😊][ans07] | 93.939 |
8 |Scheduling and Resource Allocation|[📝][sl08] | [✏️][hw08] | [😊][ans08] | 95.764 |
9 |Processing Element Design|[📝][sl09] | [✏️][hw09] | [😊][ans09] | 97.249 |
10 |SoC and DSP Architecture|[📝][sl10] | [✏️][hw10] | [😊][ans10] | - |
11 |Case Study: Motion Estimation| [📝][sl11] | - | - | - |
|12 | Case Study: Fast Fourier Transform | [📝][sl12] | - | - | - |



[sl00]:slides/0_syllabus.pdf
[sl01]:slides/1_introduction.pdf
[sl02]:slides/2_iteration_bound.pdf
[sl03]:slides/3_pipelining_and_parallel_processing.pdf
[sl04]:slides/4_retiming.pdf
[sl05]:slides/5_unfolding.pdf
[sl06]:slides/6_folding.pdf
[sl07]:slides/7_systolic_architecture_design.pdf
[sl08]:slides/8_scheduling_and_resource_allocation.pdf
[sl09]:slides/9_processing_elements_design.pdf
[sl10]:slides/10_SoC_and_DSP_Architecture.pdf
[sl11]:slides/11_hardware_architecture_of_motion_estimation.pdf
[sl12]:slides/12_case_study_FFT.pdf

[hw01]:homework/hw1.pdf
[hw02]:homework/hw2.pdf
[hw03]:homework/hw3.pdf
[hw04]:homework/hw4.pdf
[hw05]:homework/hw5.pdf
[hw06]:homework/hw6.pdf
[hw07]:homework/hw7.pdf
[hw08]:homework/hw8.pdf
[hw09]:homework/hw9.pdf
[hw10]:homework/hw10.pdf

[ans01]:answer/hw1s.pdf
[ans02]:answer/hw2s.pdf
[ans03]:answer/hw3s.pdf
[ans04]:answer/hw4s.pdf
[ans05]:answer/hw5s.pdf
[ans06]:answer/hw6s.pdf
[ans07]:answer/hw7s.pdf
[ans08]:answer/hw8s.pdf
[ans09]:answer/hw9s.pdf
[ans10]:answer/hw10s.pdf

前面九個單元算是這堂課的重頭戲，後面三個單元是以介紹為主。其中第三個單元 Pipelining and Parallel Processing 被老師稱為就算把其他上課內容都忘光了也要記得的部分。原因就是切 Pipeline 實在是一個最常見降低 ciritical path 和功率，提升頻率和 throughput 的方式。另外第七的單元也十分的重要，特別是在設計AI加速器的時候會使用到 systollic architecture，例如 Google的Tensor 晶片。每個單元其實都不會很困難（甚至稱得上簡單！？），只要上課認真聽講可以說是百分之百可以學會（第七單元有點複雜）。每個單元老師也都會利用非常簡單的範例來展示這項電路架構設計方法可以解決哪些問題或帶來哪些優勢，用非常平易近人的方式讓同學了解該技巧要如何在實際的例子中使用。因此雖然老師並沒有規定一定要實體出席，我還是很推薦可以去現場聽課因為老師講解得非常清楚。
這堂課也需要做期末專題。專題的內容就是找二到三篇“不同架構”的晶片設計論文，分析不同設計之間的取捨或好壞。因此重點是在分析上面，雖然實作會加分但重點其實並不是要寫一對的程式累死自己。這堂課要訓練的是架構層面的思考，因此比較架構之間的差異就是一個很好的起始點。

## 評分方式
作業 40%<br/>
期末考 35%<br/>
期末專題 25%

在公布的十次作業之中，最後一次不需要繳交且助教會隨附答案讓同學準備考試。其他的九次可以刪除最低分的一次。考慮到作業的難度確實不高，都是老師上課重點中的重點，想要在作業這一項拿高分實在不難。至於期末考就是直接從作業的題目抽出一模一樣的出來考，因此只要把功課的題目都掌握好像要考差應該不太可能。另外期末專題的評分也十分佛心，基本上只要不要太偏離主題都可以拿到九十以上的高分。因此雖然這堂課並不調分，但因各項原始數據都很高，我們這一屆總共有229位學生，最後有184位拿到 A+，可以說老師是非常佛心的。

## 結語
很多人會說這堂課水分很足，什麼東西都很簡單，只想應付過去就好的同學也能輕鬆 A+。不過我依然認為這堂課教授的東西確實非常實用，有興趣的同學也可以找更多的資料或例子自我學習或在期末專題上面多多發力。事實上老師也曾經是這堂課的學生，並且也是繼續延伸期末專題的研究最後成功投稿。師傅領進門，修行在個人，想要取得豐碩的成過前也得付出相應的努力，即便這些努力可能並不會體現在成績上，他們都會是未來最滋潤的養分。推薦給對數位設計有興趣，且想要練習站在架構層級思考的同學。