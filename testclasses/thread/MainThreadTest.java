package thread;

public class MainThreadTest {
    
    public static void main(String[] args) {
        Thread mainThread = Thread.currentThread();
        System.out.println("name: " + mainThread.getName());
        System.out.println("isAlive: " + mainThread.isAlive());
        System.out.println("isDaemon: " + mainThread.isDaemon());
        System.out.println("ThreadGroup: " + mainThread.getThreadGroup());
    }
}
