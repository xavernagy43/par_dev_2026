# par_dev_2026
**Parallel development Repo**


**Beadandó témája:**

C#-ban készült Mandelbrot-halmazon alapuló képgenerálás a választott feladatom. A program lényege, hogy a képpontokhoz kiszámolja, melyek tartoznak a halmazhoz, majd az eredmények alapján elkészíti a képet. A mérési eredmények a futási időt fogják reprezentálni a következő szempontok alapján:
    - workerGroup-ok nagységa szerint
    - iteráció mennyisége szerint
    - kép felbontása szerint


**Mérések:**

**Iteráció és felbontás u.a.**			
WorkerGroup mérete	Iteráció	Felbontás	Futásidő (ms)
            1	        500	    2048x1024	    854
            2	        500	    2048x1024	    534
            4	        500	    2048x1024	    392
            8	        500	    2048x1024	    322
            16	        500	    2048x1024	    266
            32	        500	    2048x1024	    258
            64	        500	    2048x1024	    241
            128	        500	    2048x1024	    249
            256	        500	    2048x1024	    250
            512	        500	    2048x1024	    262

Ha a WorkerGroup méretét növeljük és az iterációt, felbontást ugyanakkorára hagyjuk akkor látható, hogy a futásidő fokozatosan csökken. mindaddig amíg a group mérete nem kezd el túl nagy lenni.

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

Ha a WorkerGroup mérete és az iteráció ugyanakkora és a felbontást növeljük akkor látható hogy ameddig a felbontás(pixelszám) nem túl nagy számú, addig gyorsul a futásidő, de ha már túl nagy lesz akkor a futásiidő jóval elkezd romlani.

![alt text](beadando\mandelbrot\meas_images\meas3_1.png)
![alt text](beadando\mandelbrot\meas_images\meas3_2.png)
