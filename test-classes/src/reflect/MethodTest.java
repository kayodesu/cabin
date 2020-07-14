package reflect;

import java.lang.reflect.Method;
import java.util.concurrent.Callable;

/**
 * Status: Pass
 */
public class MethodTest implements Callable<Integer> {
    
    public static void main(String[] args) throws Exception {
        boxReturn();
        invokeInterfaceMethod();
    }

    public static void boxReturn() throws Exception {
        Method m = MethodTest.class.getMethod("returnLong");
        Object x = m.invoke(new MethodTest());
        System.out.println(3L == (Long) x ? "Pass" : "Fail");
    }

    public static void invokeInterfaceMethod() throws Exception {
        Method m = Callable.class.getMethod("call");
        Object x = m.invoke(new MethodTest());
        System.out.println(7 == (Integer) x ? "Pass" : "Fail");
    }

    @Override
    public Integer call() {
        return 7;
    }

    public long returnLong() {
        return 3;
    }

}
