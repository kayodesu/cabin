package thread;

public class DaemonTest {
    
    public static void main(String[] args) {
        Thread mainThread = Thread.currentThread();
     //   assertFalse(mainThread.isDaemon());

        Thread newThread = new Thread();
    //    assertFalse(newThread.isDaemon());

        newThread.setDaemon(true);
     //   assertTrue(newThread.isDaemon());
    }
}
