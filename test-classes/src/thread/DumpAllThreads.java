package thread;

import java.lang.management.ManagementFactory;
import java.lang.management.ThreadMXBean;
import java.lang.management.ThreadInfo;

class DumpAllThreads {
    public static void main(String[] args) {
        ThreadMXBean mxb = ManagementFactory.getThreadMXBean();
        // 不需要获取同步的monitor和synchronizer信息，仅获取线程和线程堆栈信息。
        ThreadInfo[] infos = mxb.dumpAllThreads(false, false);
        for (ThreadInfo info : infos) {
            System.out.println("[" + info.getThreadId() + "] " + info.getThreadName());
        }
    }
}