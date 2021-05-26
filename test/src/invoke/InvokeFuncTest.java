package invoke;

/**
 * Status: Pass
 */
public class InvokeFuncTest implements Runnable {
    public static void main(String[] args) {
        new InvokeFuncTest().test();
    }

    public void test() {
        InvokeFuncTest.staticMethod(); // invokestatic
        InvokeFuncTest demo = new InvokeFuncTest(); // invokespecial
        demo.instanceMethod(); // invokespecial
        super.equals(null); // invokespecial
        this.run(); // invokevirtual
        ((Runnable) demo).run(); // invokeinterface
    }

    public static void staticMethod() {
        System.out.println("staticMethod");
    }

    private void instanceMethod() {
        System.out.println("instanceMethod");
    }

    @Override
    public void run() {
        System.out.println("run");
    }
}
