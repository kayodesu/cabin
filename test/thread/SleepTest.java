
public class SleepTest {
    
    public static void main(String[] args)  {
        long beforeSleep = System.currentTimeMillis();
        try {
            Thread.sleep(100);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        long afterSleep = System.currentTimeMillis();
      //  assertTrue(afterSleep - beforeSleep >= 100);
    }

}
