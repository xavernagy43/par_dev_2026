# par_dev_2026
**Parallel development Repo**


**Beadandó témája:**

C#-ban készült Mandelbrot-halmazon alapuló képgenerálás a választott feladatom. A program lényege, hogy a képpontokhoz kiszámolja, melyek tartoznak a halmazhoz, majd az eredmények alapján elkészíti a képet. A mérési eredmények a futási időt fogják reprezentálni a következő szempontok alapján:
    - workerGroup-ok nagységa szerint
    - iteráció mennyisége szerint
    - kép felbontása szerint


**Mérések:**

**Szekvenciálisan**
 **2048x1024 felbontásra: ~ 697 ms**


**Iteráció és felbontás u.a.**			
WorkerGroup mérete	Iteráció	Felbontás	Futásidő (ms)  Gyorsítás (ms)   Gyorsítás(%)        
            1	        500	    2048x1024	    854           0                 0%
            2	        500	    2048x1024	    534           320               37.47%
            4	        500	    2048x1024	    392           462               54.10%
            8	        500	    2048x1024	    322           532               62.29%
            16	        500	    2048x1024	    266           588               68.85%
            32	        500	    2048x1024	    258           596               69.79%
            64	        500	    2048x1024	    241           613               71.78%
            128	        500	    2048x1024	    249           605               70.84%
            256	        500	    2048x1024	    250           604               70.73%
            512	        500	    2048x1024	    262           592               69.32%

Ha a WorkerGroup méretét növeljük és az iterációt, felbontást ugyanakkorára hagyjuk akkor látható, hogy a futásidő fokozatosan csökken. mindaddig amíg a group mérete nem kezd el túl nagy lenni. Érdemes megjegyezni,  hogy szekvenciális (CPU) futtatáshoz képest a GPU-s megoldás jelentős gyorsulást ad, mivel a pixelek számítása egymás után helyett párhuzamosan történik. Ez különösen a Mandelbrot esetében hatékony, mert minden pixel számítása független a többitől.

![alt text](beadando\mandelbrot\meas_images\meas1_1.png)
![alt text](beadando\mandelbrot\meas_images\meas1_2.png)

**Felbontás és a workerGroup u.a.**			
WorkerGroup mérete	Iteráció	Felbontás	Futásidő (ms)
            128	        50	    2048x1024	    252
            128	        100	    2048x1024	    246
            128	        200	    2048x1024	    239
            128	        300	    2048x1024	    229
            128	        500	    2048x1024	    248
            128	        800	    2048x1024	    290
            128	        1000	2048x1024	    296
            128	        1500	2048x1024	    308
            128	        2000	2048x1024	    312
            128	        2500	2048x1024	    351

Ha a WorkerGroup mérete és a felbontás ugyanakkora és az iterációt növeljük akkor látható hogy ameddig az iteráció nem túl nagy számú, addig gyorsul a futásidő, de ha már túl nagy lesz fokozatosan növekszik egyre jobban.

![alt text](beadando\mandelbrot\meas_images\meas2_1.png)
![alt text](beadando\mandelbrot\meas_images\meas2_2.png)

**Iteráció és a workerGroup u.a.**			
WorkerGroup mérete	Iteráció	Felbontás	Futásidő (ms)
        256	            500	     512x256	    240
        256	            500	    1024x512	    238
        256	            500	    1536x768	    236
        256	            500	    2048x1024	    257
        256	            500	    2560x1280	    266
        256	            500	    3072x1536	    287
        256	            500	    3584x1792	    295
        256	            500	    4096x2048	    335
        256	            500	    4608x2304	    353
        256	            500	    5120x2560	    418

Ha a WorkerGroup mérete és az iteráció ugyanakkora és a felbontást növeljük akkor látható hogy ameddig a felbontás(pixelszám) nem túl nagy számú, addig jobb a futásidő, de ha már túl nagy lesz akkor a futásidő jóval elkezd romlani.

![alt text](beadando\mandelbrot\meas_images\meas3_1.png)
![alt text](beadando\mandelbrot\meas_images\meas3_2.png)


**Összegzés:**
A mérések alapján jól látható, hogy a Mandelbrot halmazon alapuló képgenerálás hatékonyan párhuzamosítható, mivel az egyes pixelek számítása egymástól függetlenül történik. Ennek köszönhetően a GPU-s megvalósítás jelentús gyorsulást eredményez a szekvenciális (CPU) feldolgozáshoz képest.

A workerGroup méretének növelésével kezdetben jelentős teljesítménynövekedés van, azonban egy bizonyos méret felett a futásidő már nem javul tovább, sőt rosszabb is lesz. Ez a hardver korlátainak és az ütemezési többletterhelésnek tudható be.

Az iterációk számának növelése növeli a számítási igényt, így nagyobb iterációszám esetén a futásidő is növekszik. Kis iterációszámnál azonban a GPU hatékonyaan képes kihasználni a párhuzamos végrehajtást.

A felbontás növelésével a feldolgozandó pixeleknek a száma is nő, ami közvetlenül növeli a futásidőt. Ugyanakkor kisebb felbontások esetében a GPU erőforrásai nem kerülnek teljes mértékben kihasználásra, ezért ott nem feltétlenül lineáris a teljesítménynövekedés.
