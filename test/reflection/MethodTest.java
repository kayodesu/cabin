import java.util.concurrent.Callable;

public class MethodTest implements Callable<Integer> {
    
//    public static void main(String[] args) throws Exception {
//        UnitTestRunner.run(MethodTest.class);
//    }
//
//    @Test
//    public void boxReturn() throws Exception {
//        Method m = MethodTest.class.getMethod("returnLong");
//        Object x = m.invoke(new MethodTest());
//        assertEquals(3L, x);
//    }
//
//    @Test
//    public void invokeInterfaceMethod() throws Exception {
//        Method m = Callable.class.getMethod("call");
//        Object x = m.invoke(new MethodTest());
//        assertEquals(7, x);
//    }
//
    @Override
    public Integer call() {
        return 7;
    }
//
//    public long returnLong() {
//        return 3;
//    }
//
}
